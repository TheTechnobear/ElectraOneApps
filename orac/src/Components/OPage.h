#pragma once

#include "OComponent.h"
#include "OParam.h"

#include <KontrolModel.h>

class OracPage : public OComponent {
public:
  OracPage(unsigned pn) : rack_(nullptr), module_(nullptr), page_(nullptr), pageN_(pn) {}

  ~OracPage() {}

  void init() {
    for (unsigned i = 0; i < MAX_PARAMS; i++) {
      paramCtrls_[i].onClick = [this, i](void) {
        click(i);
        return true;
      };

      getParentWindow()->addAndMakeVisible(&paramCtrls_[i]);

      unsigned enc =  ((i/2) * 6) +(i % 2) + (pageN_* 2) ;
      paramCtrls_[i].setId(enc);
      getParentWindow()->assignPot(enc, 0, &paramCtrls_[i]);
    }
  }

  void click(unsigned i) { logMessage("clik param %d", i); }

  void setPage( const std::shared_ptr<Kontrol::Rack> &r,
  				const std::shared_ptr<Kontrol::Module> &m,
               	const std::shared_ptr<Kontrol::Page> &p) {
  	rack_ = r;
    module_ = m;
    page_ = p;

    auto params = module_->getParams(p);

    for (unsigned i = 0; i < MAX_PARAMS; i++) {
      if (i < params.size())
        paramCtrls_[i].setParam(rack_,module_,params[i]);
      else
        paramCtrls_[i].setParam(rack_,module_,nullptr);
    }
    repaint();
  }

  void paint(void) {
    OComponent::paint();

    for (unsigned i = 0; i < MAX_PARAMS; i++) {
      paramCtrls_[i].paint();
    }
    screen.drawRect(screenX, screenY, width - 1, height - 1, fgClr_);

    if (module_ && page_) {
      auto &name = page_->displayName();

      screen.printText(screenX + 1, screenY + 1, name.c_str(),
                       TextStyle::smallWhiteOnBlack, width - 2,
                       TextAlign::center);
    }

    if (active_) {
      // screen.drawLine(screenX - 1, screenY - 1, width, 1, 0xFFFF);
    }
  }

  void setBounds(uint16_t newX, uint16_t newY, uint16_t newWidth,
                 uint16_t newHeight) {
    OComponent::setBounds(newX, newY, newWidth, newHeight);

    int sp = 20;
    int nameH = 0;
    int d = (height - nameH - (sp * 3)) / 2;
    int x = screenX + sp;
    int y = screenY + sp;

    paramCtrls_[0].setBounds(x, y, d, d);
    paramCtrls_[1].setBounds(x + d + sp, y, d, d);
    paramCtrls_[2].setBounds(x, y + d + sp, d, d);
    paramCtrls_[3].setBounds(x + d + sp, y + d + sp, d, d);
  }

  std::shared_ptr<Kontrol::Module> module() { return module_; }
  std::shared_ptr<Kontrol::Page> page() { return page_; }

private:
  static constexpr unsigned MAX_PARAMS = 4;
  OracParam paramCtrls_[MAX_PARAMS];
  std::shared_ptr<Kontrol::Rack> rack_;
  std::shared_ptr<Kontrol::Module> module_;
  std::shared_ptr<Kontrol::Page> page_;
  unsigned pageN_ = 0;
};
