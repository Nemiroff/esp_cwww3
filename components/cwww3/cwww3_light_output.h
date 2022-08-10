#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace cwww3 {

class CWWW3LightOutput : public light::LightOutput {
 public:
  void set_cold_white(output::FloatOutput *cold_white) { cold_white_ = cold_white; }
  void set_warm_white(output::FloatOutput *warm_white) { warm_white_ = warm_white; }
  void set_cold_white_temperature(float cold_white_temperature) { cold_white_temperature_ = cold_white_temperature; }
  void set_warm_white_temperature(float warm_white_temperature) { warm_white_temperature_ = warm_white_temperature; }
  void set_brightness_output(output::FloatOutput *brightness_output) { brightness_output_ = brightness_output; }
  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
    traits.set_min_mireds(this->cold_white_temperature_);
    traits.set_max_mireds(this->warm_white_temperature_);
    return traits;
  }
  void write_state(light::LightState *state) override {
    float cwhite, wwhite, bright;
    state->current_values_as_brightness(&bright);
    const float temperature = clamp(state->current_values.get_color_temperature(), cold_white_temperature_, warm_white_temperature_);
    wwhite = (temperature - cold_white_temperature_) / (warm_white_temperature_ - cold_white_temperature_);
    cwhite = (temperature - warm_white_temperature_) / (cold_white_temperature_ - warm_white_temperature_);
    const float max_cw_ww = std::max(wwhite, cwhite);
    cwhite /= max_cw_ww;
    wwhite /= max_cw_ww;
    if (bright < 0.000004f){
        cwhite = 0.0f;
        wwhite = 0.0f;
        bright = 0.0f;
        state->current_values.set_state(false);
    } else if (bright < 0.0175f){
        bright = 0.0175f;
    }
    this->cold_white_->set_level(cwhite);
    this->warm_white_->set_level(wwhite);
    this->brightness_output_->set_level(bright);
  }

 protected:
  output::FloatOutput *cold_white_;
  output::FloatOutput *warm_white_;
  output::FloatOutput *brightness_output_;
  float cold_white_temperature_{0};
  float warm_white_temperature_{0};
};

}  // namespace cwww3
}  // namespace esphome
