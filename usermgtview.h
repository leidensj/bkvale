#ifndef USERMGTVIEW_H
#define USERMGTVIEW_H

#include <QFrame>
#include "user.h"

class QCheckBox;
class JLineEdit;
class QPushButton;
class QLabel;

class UserMgtView : public QFrame
{
  Q_OBJECT

public:
  explicit UserMgtView(QWidget* parent = 0);
  User getUser() const;
  void setUser(const User& user);
  QString getPassword() const;

private:
  int m_currentID;
  QPushButton* m_create;
  QPushButton* m_save;
  JLineEdit* m_user;
  QLabel* m_lblPasswordMsg;
  JLineEdit* m_password;
  QPushButton* m_viewPassword;
  QCheckBox* m_accessNote;
  QCheckBox* m_accessReminder;
  QCheckBox* m_accessCalculator;
  QCheckBox* m_accessShop;
  QCheckBox* m_accessConsumption;
  QCheckBox* m_accessUser;
  QCheckBox* m_accessProduct;
  QCheckBox* m_accessSettings;

public slots:
  void create();

private slots:
  void emitSaveSignal();
  void viewPassword(bool b);

signals:
  saveSignal();
};

#endif // USERMGTVIEW_H
