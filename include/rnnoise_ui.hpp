/*
 *  Copyright © 2017-2022 Wellington Wallace
 *
 *  This file is part of EasyEffects.
 *
 *  EasyEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EasyEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <adwaita.h>
#include <filesystem>
#include "application.hpp"
#include "effects_base.hpp"
#include "ui_helpers.hpp"

namespace ui::rnnoise_box {

G_BEGIN_DECLS

#define EE_TYPE_RNNOISE_BOX (rnnoise_box_get_type())

G_DECLARE_FINAL_TYPE(RNNoiseBox, rnnoise_box, EE, RNNOISE_BOX, GtkBox)

G_END_DECLS

auto create() -> RNNoiseBox*;

void setup(RNNoiseBox* self,
           std::shared_ptr<RNNoise> rnnoise,
           const std::string& schema_path,
           app::Application* application);

}  // namespace ui::rnnoise_box

#include <glibmm/i18n.h>
#include "plugin_ui_base.hpp"

class RNNoiseUi : public Gtk::Box, public PluginUiBase {
 public:
  RNNoiseUi(BaseObjectType* cobject,
            const Glib::RefPtr<Gtk::Builder>& builder,
            const std::string& schema,
            const std::string& schema_path);
  RNNoiseUi(const RNNoiseUi&) = delete;
  auto operator=(const RNNoiseUi&) -> RNNoiseUi& = delete;
  RNNoiseUi(const RNNoiseUi&&) = delete;
  auto operator=(const RNNoiseUi&&) -> RNNoiseUi& = delete;
  ~RNNoiseUi() override;

  static auto add_to_stack(Gtk::Stack* stack, const std::string& schema_path) -> RNNoiseUi*;

 private:
  inline static const std::string log_tag = "rnnoise_ui: ";

  const std::string rnnn_ext = ".rnnn";

  Glib::ustring default_model_name;

  Gtk::Button* import_model = nullptr;

  Gtk::Frame* model_list_frame = nullptr;

  Gtk::ListView* listview = nullptr;

  Gtk::Label* active_model_name = nullptr;

  Glib::RefPtr<Gtk::StringList> string_list;

  Glib::RefPtr<Gio::FileMonitor> folder_monitor;

  std::filesystem::path model_dir;

  void setup_listview();

  void on_selection_changed();
};
