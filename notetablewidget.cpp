#include "notetablewidget.h"
#include "databaseutils.h"
#include <QHeaderView>
#include <QKeyEvent>
#include "jtablewidgetitem.h"

NoteTableWidget::NoteTableWidget(QWidget* parent)
  : JTable(parent)
{
  setColumnCount(NOTE_NUMBER_OF_COLUMNS);
  QStringList headers;
  headers << "Quantidade" << "Unidade" << "Produto" << "Preço" << "Subtotal";
  setHorizontalHeaderLabels(headers);


  setSelectionBehavior(QAbstractItemView::SelectItems);
  setSelectionMode(QAbstractItemView::SingleSelection);
  horizontalHeader()->setHighlightSections(false);
  verticalHeader()->setHighlightSections(false);
  verticalHeader()->setSectionsMovable(true);
  horizontalHeader()->setSectionResizeMode((int)NoteColumn::Ammount, QHeaderView::ResizeToContents);
  horizontalHeader()->setSectionResizeMode((int)NoteColumn::Unity, QHeaderView::ResizeToContents);
  horizontalHeader()->setSectionResizeMode((int)NoteColumn::Description, QHeaderView::Stretch);
  horizontalHeader()->setSectionResizeMode((int)NoteColumn::Price, QHeaderView::ResizeToContents);
  horizontalHeader()->setSectionResizeMode((int)NoteColumn::SubTotal, QHeaderView::ResizeToContents);
}

double NoteTableWidget::computePrice(int row) const
{
  const double ammount = ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::Ammount))->getValue();
  const double subTotal = ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::SubTotal))->getValue();
  const double price = ammount ? subTotal / ammount : 0.0;
  return price;
}

double NoteTableWidget::computeSubTotal(int row) const
{
  const double ammount = ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::Ammount))->getValue();
  const double price = ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::Price))->getValue();
  const double subTotal = ammount * price;
  return subTotal;
}

double NoteTableWidget::computeTotal() const
{
  double total = 0.0;
  for (int row = 0; row != rowCount(); ++row)
    total += ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::SubTotal))->getValue();
  return total;
}

const JItem& NoteTableWidget::getItem(int row) const
{
  m_ref.clear();
  if (isValidRow(row))
  {
    int idx = verticalHeader()->logicalIndex(row);
    m_ref.m_ammount = ((DoubleTableWidgetItem*)item(idx, (int)NoteColumn::Ammount))->getValue();
    m_ref.m_price = ((DoubleTableWidgetItem*)item(idx, (int)NoteColumn::Price))->getValue();
    m_ref.m_package = ((PackageTableWidgetItem*)item(idx, (int)NoteColumn::Unity))->getItem();
    m_ref.m_product = dynamic_cast<const Product&>(((ProductTableWidgetItem*)item(idx, (int)NoteColumn::Description))->getItem());
  }
  return m_ref;
}

void NoteTableWidget::addItemAndLoadPrices(qlonglong supplierId)
{
  addItem(NoteItem());
  int row = rowCount() - 1;
  auto ptProductCell = dynamic_cast<ProductTableWidgetItem*>(item(row, (int)NoteColumn::Description));
  ptProductCell->selectItem(PRODUCT_FILTER_NOTE);
  auto product = dynamic_cast<const Product&>(ptProductCell->getItem());
  NoteItem noteItem;
  if (IS_VALID_ID(supplierId))
    noteItem = NoteSQL::selectLastItem(supplierId, product.m_id);
  auto ptPriceCell = dynamic_cast<DoubleTableWidgetItem*>(item(row, (int)NoteColumn::Price));
  ptPriceCell->setValue(noteItem.m_price);
  auto ptPackageCell = dynamic_cast<PackageTableWidgetItem*>(item(row, (int)NoteColumn::Unity));
  ptPackageCell->setItem(noteItem.m_package, product.m_unity);
}

void NoteTableWidget::addItemAndLoadPricesByBarcode(qlonglong supplierId)
{
  addItem(NoteItem());
  int row = rowCount() - 1;
  auto ptProductCell = dynamic_cast<ProductTableWidgetItem*>(item(row, (int)NoteColumn::Description));
  ptProductCell->selectItemByBarcode(PRODUCT_FILTER_NOTE);
  NoteItem noteItem;
  auto product = dynamic_cast<const Product&>(ptProductCell->getItem());
  if (IS_VALID_ID(supplierId))
    noteItem = NoteSQL::selectLastItem(supplierId, product.m_id);
  auto ptPriceCell = dynamic_cast<DoubleTableWidgetItem*>(item(row, (int)NoteColumn::Price));
  ptPriceCell->setValue(noteItem.m_price);
  auto ptPackageCell = dynamic_cast<PackageTableWidgetItem*>(item(row, (int)NoteColumn::Unity));
  ptPackageCell->setItem(noteItem.m_package, product.m_unity);
}

void NoteTableWidget::addItem(const JItem& o)
{
  auto _o = dynamic_cast<const NoteItem&>(o);

  blockSignals(true);

  insertRow(rowCount());
  int row = rowCount() - 1;
  setItem(row, (int)NoteColumn::Ammount, new DoubleTableWidgetItem(JItem::DataType::Ammount,
                                                                   DoubleTableWidgetItem::Color::Background));
  setItem(row, (int)NoteColumn::Unity, new PackageTableWidgetItem);
  setItem(row, (int)NoteColumn::Description, new ProductTableWidgetItem);
  setItem(row, (int)NoteColumn::Price, new DoubleTableWidgetItem(JItem::DataType::Money,
                                                                 DoubleTableWidgetItem::Color::Background));
  setItem(row, (int)NoteColumn::SubTotal, new DoubleTableWidgetItem(JItem::DataType::Money,
                                                                    DoubleTableWidgetItem::Color::Foreground));
  setCurrentCell(row, (int)NoteColumn::Ammount);

  ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::Ammount))->setValue(_o.m_ammount);
  ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::Price))->setValue(_o.m_price);
  ((DoubleTableWidgetItem*)item(row, (int)NoteColumn::SubTotal))->setValue(_o.subtotal());
  ((ProductTableWidgetItem*)item(row, (int)NoteColumn::Description))->setItem(_o.m_product);
  ((PackageTableWidgetItem*)item(row, (int)NoteColumn::Unity))->setItem(_o.m_package, _o.m_product.m_unity);

  blockSignals(false);
}


void NoteTableWidget::update(int row, int column)
{
  blockSignals(true);
  switch ((NoteColumn)column)
  {
    case NoteColumn::Ammount:
    {
      auto ptAmmount = (DoubleTableWidgetItem*)item(row, column);
      ptAmmount->evaluate();
      auto ptSubtotal = (DoubleTableWidgetItem*)item(row, (int)NoteColumn::SubTotal);
      ptSubtotal->setValue(computeSubTotal(row));
    } break;
    case NoteColumn::Price:
    {
      auto ptPrice = (DoubleTableWidgetItem*)item(row, column);
      ptPrice->evaluate();
      auto ptSubtotal = (DoubleTableWidgetItem*)item(row, (int)NoteColumn::SubTotal);
      ptSubtotal->setValue(computeSubTotal(row));
    } break;
    case NoteColumn::SubTotal:
    {
      auto ptSubtotal = (DoubleTableWidgetItem*)item(row, column);
      ptSubtotal->evaluate();
      auto ptPrice = (DoubleTableWidgetItem*)item(row, (int)NoteColumn::Price);
      ptPrice->setValue(computePrice(row));
      //Re-compute subtotal
      ptSubtotal->setValue(computeSubTotal(row));
    } break;
    default:
      break;
  }

  blockSignals(false);
  emitChangedSignal();
}

void NoteTableWidget::itemDoubleClicked(int row, int column)
{
  if (column == (int)NoteColumn::Description)
  {
    auto ptProduct = (ProductTableWidgetItem*)item(row, column);
    ptProduct->selectItem(PRODUCT_FILTER_NOTE);
    auto ptPackage = (PackageTableWidgetItem*)item(row, (int)NoteColumn::Unity);
    ptPackage->setItem(Package(), dynamic_cast<const Product&>(ptProduct->getItem()).m_unity);
  }
  else if (column == (int)NoteColumn::Unity)
  {
    auto ptProduct = (ProductTableWidgetItem*)item(row, (int)NoteColumn::Description);
    auto ptPackage = (PackageTableWidgetItem*)item(row, column);
    ptPackage->selectItem(dynamic_cast<const Product&>(ptProduct->getItem()).m_unity);
  }
}
