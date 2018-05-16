#include <gst/insertbin/gstinsertbin.h>
#include <cmath>
#include "limiter.hpp"
#include "util.hpp"

namespace {

void on_state_changed(GSettings* settings, gchar* key, Limiter* l) {
    auto enable = g_settings_get_boolean(settings, "state");
    auto plugin = gst_bin_get_by_name(GST_BIN(l->plugin), "limiter_bin");

    if (enable) {
        if (!plugin) {
            gst_insert_bin_append(
                GST_INSERT_BIN(l->plugin), l->bin,
                [](auto bin, auto elem, auto success, auto d) {
                    auto l = static_cast<Limiter*>(d);

                    if (success) {
                        util::debug(l->log_tag + "limiter enabled");
                    } else {
                        util::debug(l->log_tag +
                                    "failed to enable the limiter");
                    }
                },
                l);
        }
    } else {
        if (plugin) {
            gst_insert_bin_remove(
                GST_INSERT_BIN(l->plugin), l->bin,
                [](auto bin, auto elem, auto success, auto d) {
                    auto l = static_cast<Limiter*>(d);

                    if (success) {
                        util::debug(l->log_tag + "limiter disabled");
                    } else {
                        util::debug(l->log_tag +
                                    "failed to disable the limiter");
                    }
                },
                l);
        }
    }
}

}  // namespace

Limiter::Limiter(std::string tag, std::string schema)
    : log_tag(tag), settings(g_settings_new(schema.c_str())) {
    limiter = gst_element_factory_make("calf-sourceforge-net-plugins-Limiter",
                                       nullptr);

    plugin = gst_insert_bin_new("limiter_plugin");

    if (limiter != nullptr) {
        is_installed = true;
    } else {
        is_installed = false;

        util::warning("Limiter plugin was not found!");
    }

    if (is_installed) {
        bin = gst_insert_bin_new("limiter_bin");

        auto input_level =
            gst_element_factory_make("level", "limiter_input_level");
        auto output_level =
            gst_element_factory_make("level", "limiter_output_level");

        autovolume = gst_element_factory_make("level", "autovolume");

        gst_insert_bin_append(GST_INSERT_BIN(bin), input_level, nullptr,
                              nullptr);
        gst_insert_bin_append(GST_INSERT_BIN(bin), limiter, nullptr, nullptr);
        gst_insert_bin_append(GST_INSERT_BIN(bin), output_level, nullptr,
                              nullptr);
        gst_insert_bin_append(GST_INSERT_BIN(bin), autovolume, nullptr,
                              nullptr);

        bind_to_gsettings();

        g_signal_connect(settings, "changed::state",
                         G_CALLBACK(on_state_changed), this);

        // useless write just to force callback call

        auto enable = g_settings_get_boolean(settings, "state");

        g_settings_set_boolean(settings, "state", enable);
    }
}

Limiter::~Limiter() {}

void Limiter::bind_to_gsettings() {
    g_settings_bind_with_mapping(
        settings, "input-gain", limiter, "level-in", G_SETTINGS_BIND_DEFAULT,
        util::db20_gain_to_linear, util::linear_gain_to_db20, nullptr, nullptr);

    g_settings_bind_with_mapping(settings, "limit", limiter, "limit",
                                 G_SETTINGS_BIND_GET, util::db20_gain_to_linear,
                                 nullptr, nullptr, nullptr);

    // calf limiter does automatic makeup gain by the same amount given as
    // limit. See https://github.com/calf-studio-gear/calf/issues/162
    // that is why we reduce the output level accordingly

    g_settings_bind_with_mapping(settings, "limit", limiter, "level-out",
                                 G_SETTINGS_BIND_GET, util::db20_gain_to_linear,
                                 nullptr, nullptr, nullptr);

    g_settings_bind_with_mapping(settings, "lookahead", limiter, "attack",
                                 G_SETTINGS_BIND_GET, util::double_to_float,
                                 nullptr, nullptr, nullptr);

    g_settings_bind_with_mapping(settings, "release", limiter, "release",
                                 G_SETTINGS_BIND_GET, util::double_to_float,
                                 nullptr, nullptr, nullptr);

    g_settings_bind(settings, "asc", limiter, "asc", G_SETTINGS_BIND_DEFAULT);

    g_settings_bind_with_mapping(settings, "asc-level", limiter, "asc-coeff",
                                 G_SETTINGS_BIND_GET, util::double_to_float,
                                 nullptr, nullptr, nullptr);

    g_settings_bind(settings, "oversampling", limiter, "oversampling",
                    G_SETTINGS_BIND_DEFAULT);

    g_settings_bind_with_mapping(settings, "autovolume-window", autovolume,
                                 "interval", G_SETTINGS_BIND_GET,
                                 util::ms_to_ns, nullptr, nullptr, nullptr);
}

void Limiter::on_new_autovolume_level(const std::array<double, 2>& peak) {
    float attenuation, gain;

    auto max_value = (peak[0] > peak[1]) ? peak[0] : peak[1];
    auto target = g_settings_get_int(settings, "autovolume-target");
    auto tolerance = g_settings_get_int(settings, "autovolume-tolerance");

    g_object_get(limiter, "att", &attenuation, nullptr);
    g_object_get(limiter, "level-in", &gain, nullptr);

    gain = 20 * log10(gain);

    if (max_value > target + tolerance || attenuation < 1) {
        if (gain - 1 >= -36) {  // -36 = minimum input gain
            gain--;
        }
    } else if (max_value < target - tolerance) {
        if (gain + 1 <= 36) {  // 36 = maximum input gain
            gain++;
        }
    }

    gain = pow(10, gain / 20.0);

    g_object_set(limiter, "level-in", gain, nullptr);
}
