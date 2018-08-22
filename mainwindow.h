#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTcpSocket>
#include "settingsdlg.h"
#include "databaseutils.h"

namespace Ui {
class BaitaAssistant;
}

enum class Functionality : int
{
  None = 0,
  Note,
  Reminder,
  Calculator,
  Shop,
  Reservation
};

class QLabel;
class NoteView;
class ReminderView;
class ConsumptionWidget;
class CalculatorWidget;
class ShopView;
class ReservationView;
class JMdiSubWindow;

class BaitaAssistant : public QMainWindow
{
  Q_OBJECT

public:
  explicit BaitaAssistant(const UserLoginSQL& userLogin, QWidget *parent = 0);
  ~BaitaAssistant();

protected:
  void closeEvent(QCloseEvent* event);

private:
  Ui::BaitaAssistant *ui;
  UserLoginSQL m_userLogin;
  NoteView* m_note;
  ReminderView* m_reminder;
  ConsumptionWidget* m_consumption;
  CalculatorWidget* m_calculator;
  ShopView* m_shop;
  ReservationView* m_reservation;
  QSerialPort m_printerSerial;
  QTcpSocket m_printerTCP;
  Settings m_settings;
  QLabel* m_statusDatabasePath;
  QLabel* m_statusUserName;
  JMdiSubWindow* m_noteWindow;
  JMdiSubWindow* m_reminderWindow;
  JMdiSubWindow* m_calculatorWindow;
  JMdiSubWindow* m_shopWindow;
  JMdiSubWindow* m_reservationWindow;
  bool connectPrinter();
  void disconnectPrinter();
  Functionality getCurrentFunctionality() const;

private slots:
  void updateControls();
  void updateStatusBar();
  void showInfo();
  void print();
  bool print(const QString& text);
  void openSettingsDialog();
  void openUsersDialog();
  void openProductsDialog();
  void openCategoriesDialog();
  void openLoginDialog();
  void openImagesDialog();
  void openPersonsDialog();
  void openShoppingListDialog();
  void openActiveUsersDialog();
  void openDiscountMgtDialog();
  void openProductBarcodeDialog();
  void activateWindow();
};

#endif // MAINWINDOW_H
