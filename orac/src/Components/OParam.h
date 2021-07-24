#pragma once

#include "OComponent.h"
#include <KontrolModel.h>

class OracParam : public OComponent {
public:
  OracParam() {}

  ~OracParam() {}

  void setParam(
			  	const std::shared_ptr<Kontrol::Rack> &r,
  				const std::shared_ptr<Kontrol::Module> &m,
                const std::shared_ptr<Kontrol::Parameter> &p) {
  	rack_=r;
  	module_=m;
    param_ = p;
    repaint();
  }

  void paint(void) {
    OComponent::paint();

    uint16_t nameH = 12;
    int r = (height - 1 - nameH) / 2;
    // screen.drawRect(screenX, screenY, width - 1, height - 1, COLOR_WHITE);

    int xc = screenX + 1 + r + (nameH / 2);
    int yc = screenY + 1 + r;

    screen.drawCircle(xc, yc, r, COLOR_WHITE);

    float value = 0.0f;

    if (param_) {
      auto &name = param_->displayName();
      int y = screenY + height - 2 - nameH;
      screen.printText(screenX + 1, y, name.c_str(),
                       TextStyle::smallWhiteOnBlack, width - 2,
                       TextAlign::center);
      value = param_->asFloat(param_->current());
    }

    float ph = value + 0.25f;

    int xe = (cosf(ph * 2 * PI) * float(r));
    int ye = (sinf(ph * 2 * PI) * float(r));
    screen.drawLine(xc, yc, xe, ye, COLOR_WHITE);
  }

  void onPotChange(int16_t relativeChange, handle_t handle = 0) override {
    OComponent::onPotChange(relativeChange, handle);
    if (rack_&& module_ && param_) {
      auto model = Kontrol::KontrolModel::model();
      float chg = float(relativeChange/256.0f);
      // logMessage("change %d %f",relativeChange, chg);
      Kontrol::ParamValue calc = param_->calcRelative(chg);
      model->changeParam(Kontrol::CS_LOCAL, rack_->id(), module_->id(),
                         param_->id(), calc);
      repaint();
    }
  }

private:
  std::shared_ptr<Kontrol::Rack> rack_=nullptr;
  std::shared_ptr<Kontrol::Module> module_=nullptr;
  std::shared_ptr<Kontrol::Parameter> param_=nullptr;
};
