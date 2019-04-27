#ifndef STOREVIEW_H
#define STOREVIEW_H

#include "store.h"
#include "jitemview.h"

class QListWidget;
class FormInfoWidget;
class FormDetailsWidget;
class FormPhoneWidget;
class FormAddressWidget;

class StoreView : public JItemView
{
  Q_OBJECT

public:
  explicit StoreView(QWidget* parent = 0);
  const JItemSQL& getItem() const;
  Id getId() const;

public slots:
  void create();
  void updateControls();

private:
  mutable Store m_ref;
  FormInfoWidget* m_formInfo;
  FormDetailsWidget* m_formDetails;
  FormPhoneWidget* m_formPhone;
  FormAddressWidget* m_formAddress;
  QListWidget* m_list;
  void setItem(const JItemSQL& o);
};

#endif // STOREVIEW_H
