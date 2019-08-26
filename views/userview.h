#ifndef USERVIEW_H
#define USERVIEW_H

#include <QFrame>
#include "items/user.h"
#include "widgets/jitemview.h"

class QCheckBox;
class JLineEdit;
class QLabel;
class QListWidget;

class UserView : public JItemView
{
  Q_OBJECT

public:
  explicit UserView(Id currentLoggedId, QWidget* parent = 0);


  QString getPassword() const;
  bool hasLoggedUserChanged() const;

public slots:
  void getItem(JItemSQL& o) const;

protected slots:
  void itemsRemoved(const QVector<Id>& ids);
  void save();
  void setItem(const JItemSQL& o);

private:
  enum class Idx : int
  {
    User =0,
    Category,
    Product,
    Image,
    Form,
    Employee,
    Supplier,
    Store,
    Purchase,
    Calculator,
    Reminder,
    ShoppingList,
    Shop,
    Discount,
    TimeCard,
    Settings
  };

  static QString idxToTableName(Idx idx);

  Id m_currentLoggedId;
  bool m_bHasLoggedUserChanged;
  JLineEdit* m_user;
  QLabel* m_lblPasswordMsg;
  JLineEdit* m_password;
  QPushButton* m_viewPassword;
  QListWidget* m_list;

private slots:
  void viewPassword(bool b);
};

#endif // USERVIEW_H