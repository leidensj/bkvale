#ifndef PAYMENTTABLE_H
#define PAYMENTTABLE_H

#include "widgets/jtable.h"
#include "items/paymentelement.h"

class PaymentTable : public JTable
{
  Q_OBJECT

public:
  enum class Column
  {
    Date,
    Value,
  };

  explicit PaymentTable(JAddRemoveButtons* btns = nullptr, bool bSelector = false, QWidget* parent = nullptr);
  void getPaymentElements(QVector<PaymentElement>& v) const;
  void setPaymentElements(const QVector<PaymentElement>& v);
  void setPurchaseDate(const QDate& dt);
  QDate getPurchaseDate() const;
  void setPurchaseTotal(double total);
  double getPurchaseTotal() const;
  bool isValid() const;
  double total() const;

public slots:
  void addRow();
  void fill();

private:
  QDate m_dtPurchase;
  double m_purchaseTotal;
};

#endif // PAYMENTTABLE_H