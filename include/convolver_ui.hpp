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

#ifndef CONVOLVER_UI_HPP
#define CONVOLVER_UI_HPP

#include <fftw3.h>
#include <glibmm/i18n.h>
#include <algorithm>
#include <execution>
#include <filesystem>
#include <mutex>
#include <numbers>
#include <sndfile.hh>
#include <tuple>
#include "plot.hpp"
#include "plugin_ui_base.hpp"
#include "resampler.hpp"

class ConvolverUi : public Gtk::Box, public PluginUiBase {
 public:
  ConvolverUi(BaseObjectType* cobject,
              const Glib::RefPtr<Gtk::Builder>& builder,
              const std::string& schema,
              const std::string& schema_path);
  ConvolverUi(const ConvolverUi&) = delete;
  auto operator=(const ConvolverUi&) -> ConvolverUi& = delete;
  ConvolverUi(const ConvolverUi&&) = delete;
  auto operator=(const ConvolverUi&&) -> ConvolverUi& = delete;
  ~ConvolverUi() override;

  static auto add_to_stack(Gtk::Stack* stack, const std::string& schema_path) -> ConvolverUi*;

  void reset() override;

 private:
  const std::string log_tag = "convolver_ui: ";

  const std::string irs_ext = ".irs";

  Gtk::SpinButton* ir_width = nullptr;

  Gtk::ListView* listview = nullptr;

  Gtk::ScrolledWindow* scrolled_window = nullptr;

  Gtk::Button* import = nullptr;

  Gtk::Button* button_combine_kernels = nullptr;

  Gtk::DrawingArea* drawing_area = nullptr;

  Gtk::Label *label_file_name = nullptr, *label_sampling_rate = nullptr, *label_samples = nullptr,
             *label_duration = nullptr;

  Gtk::ToggleButton* show_fft = nullptr;

  Gtk::CheckButton *check_left = nullptr, *check_right = nullptr;

  Gtk::SearchEntry* entry_search = nullptr;

  Gtk::Entry* combined_kernel_name = nullptr;

  Gtk::Popover* popover_import = nullptr;

  Gtk::Popover* popover_combine = nullptr;

  Gtk::DropDown* dropdown_kernel_1 = nullptr;

  Gtk::DropDown* dropdown_kernel_2 = nullptr;

  Gtk::Spinner* spinner_combine_kernel = nullptr;

  std::filesystem::path irs_dir;

  Glib::RefPtr<Gio::FileMonitor> folder_monitor;

  std::vector<float> left_mag, right_mag, time_axis;
  std::vector<float> left_spectrum, right_spectrum, freq_axis;

  Glib::RefPtr<Gtk::StringList> string_list;

  std::unique_ptr<Plot> plot;

  Glib::RefPtr<Gio::Settings> spectrum_settings;

  std::mutex lock_guard_irs_info;

  std::vector<std::thread> mythreads;

  void setup_listview();

  static void setup_dropdown_kernels(Gtk::DropDown* dropdown, const Glib::RefPtr<Gtk::StringList>& string_list);

  auto get_irs_names() -> std::vector<Glib::ustring>;

  void import_irs_file(const std::string& file_path);

  void remove_irs_file(const std::string& name);

  void on_import_irs_clicked();

  void get_irs_info();

  void get_irs_spectrum(const int& rate);

  void plot_waveform();

  void plot_fft();

  auto read_kernel(const std::string& file_name) -> std::tuple<int, std::vector<float>, std::vector<float>>;

  void combine_kernels(const std::string& kernel_1_name,
                       const std::string& kernel_2_name,
                       const std::string& output_file_name);

  static void direct_conv(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& c);
};

#endif
