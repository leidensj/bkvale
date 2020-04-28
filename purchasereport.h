#ifndef PURCHASEREPORT_H
#define PURCHASEREPORT_H

#include <QWidget>

class QPushButton;
class QTextEdit;
class PurchaseFilter;
class QRadioButton;

class PurchaseReport : public QWidget
{
  Q_OBJECT

public:
  explicit PurchaseReport(PurchaseFilter* filter, QWidget* parent = nullptr);

private slots:

  void process();
  void processPurchase();
  void processProduct();
  void updateControls();
  void saveAsPdf();

private:
  const PurchaseFilter* m_filter;
  QPushButton* m_btnProcess;
  QPushButton* m_btnPrint;
  QPushButton* m_btnPdf;
  QTextEdit* m_report;
  QRadioButton* m_rdoPurchase;
  QRadioButton* m_rdoProduct;
  QRadioButton* m_rdoSupplier;
  QString getQuery() const;
};

#endif // PURCHASEREPORT_H
