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
#include "plot.hpp"

#include <giomm.h>
#include <memory>

#include "util.hpp"

class SpectrumUi : public Gtk::DrawingArea {
 public:
  SpectrumUi(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
  SpectrumUi(const SpectrumUi&) = delete;
  auto operator=(const SpectrumUi&) -> SpectrumUi& = delete;
  SpectrumUi(const SpectrumUi&&) = delete;
  auto operator=(const SpectrumUi&&) -> SpectrumUi& = delete;
  ~SpectrumUi() override;

  static auto add_to_box(Gtk::Box* box) -> SpectrumUi*;

  void on_new_spectrum(uint rate, uint n_bands, std::vector<float> magnitudes);

 private:
  inline static const std::string log_tag = "spectrum_ui: ";

  Glib::RefPtr<Gio::Settings> settings;

  std::unique_ptr<Plot> plot;

  std::vector<sigc::connection> connections;

  uint rate = 0U, n_bands = 0U;

  std::vector<float> spectrum_mag, spectrum_freqs, spectrum_x_axis;

  std::vector<uint> spectrum_bin_count;

  void init_frequency_axis();
};
