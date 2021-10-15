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

#ifndef MULTIBAND_COMPRESSOR_HPP
#define MULTIBAND_COMPRESSOR_HPP

#include "lv2_wrapper.hpp"
#include "plugin_base.hpp"

class MultibandCompressor : public PluginBase {
 public:
  MultibandCompressor(const std::string& tag,
                      const std::string& schema,
                      const std::string& schema_path,
                      PipeManager* pipe_manager);
  MultibandCompressor(const MultibandCompressor&) = delete;
  auto operator=(const MultibandCompressor&) -> MultibandCompressor& = delete;
  MultibandCompressor(const MultibandCompressor&&) = delete;
  auto operator=(const MultibandCompressor&&) -> MultibandCompressor& = delete;
  ~MultibandCompressor() override;

  static constexpr uint n_bands = 8U;

  void setup() override;

  void process(std::span<float>& left_in,
               std::span<float>& right_in,
               std::span<float>& left_out,
               std::span<float>& right_out,
               std::span<float>& probe_left,
               std::span<float>& probe_right) override;

  sigc::signal<void(const float&)> latency;

  sigc::signal<void(const std::array<float, n_bands>&)> reduction, envelope, curve, frequency_range;

  float latency_port_value = 0.0F;

  std::array<float, n_bands> frequency_range_end_port_array = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  std::array<float, n_bands> envelope_port_array = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  std::array<float, n_bands> curve_port_array = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
  std::array<float, n_bands> reduction_port_array = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};

 private:
  uint latency_n_frames = 0U;

  std::unique_ptr<lv2::Lv2Wrapper> lv2_wrapper;

  std::vector<pw_proxy*> list_proxies;
};

#endif
