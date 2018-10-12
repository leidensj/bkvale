#ifndef PRODUCTVIEW_H
#define PRODUCTVIEW_H

#include <QFrame>
#include "product.h"
#include "category.h"
#include "jitemview.h"

class QLabel;
class JLineEdit;
class JDoubleSpinBox;
class QCheckBox;
class JDatabasePicker;

class ProductView : public JItemView
{
  Q_OBJECT

public:
  explicit ProductView(QWidget* parent = 0);
  ~ProductView();
  const JItem& getItem() const;

public slots:
  void create();

private:
  JLineEdit* m_edName;
  JLineEdit* m_edUnity;
  JLineEdit* m_edDetails;
  QCheckBox* m_cbAvailableAtNotes;
  QCheckBox* m_cbAvailableAtShop;
  QCheckBox* m_cbAvailableAtConsumption;
  QCheckBox* m_cbAvailableToBuy;
  QCheckBox* m_cbAvailableToSell;
  JDatabasePicker* m_categoryPicker;
  JDatabasePicker* m_imagePicker;
  void setItem(const JItem& o);
};

#endif // PRODUCTVIEW_H
