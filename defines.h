#ifndef DEFINES_H
#define DEFINES_H

#define INVALID_ID 0
#define IS_VALID_ID(x) (x != INVALID_ID ? true : false)

#define SQL_COLID "_ID"
#define SQL_COLID_NUMBER 0

#define POSTGRE_CONNECTION_NAME "POSTGRE"

#define ADDRESS_SQL_TABLE_NAME "_ADDRESSES"
#define ADDRESS_SQL_COL01 "_PERSONID"
#define ADDRESS_SQL_COL02 "_CEP"
#define ADDRESS_SQL_COL03 "_NEIGHBORHOOD"
#define ADDRESS_SQL_COL04 "_STREET"
#define ADDRESS_SQL_COL05 "_NUMBER"
#define ADDRESS_SQL_COL06 "_CITY"
#define ADDRESS_SQL_COL07 "_STATE"
#define ADDRESS_SQL_COL08 "_COMPLEMENT"
#define ADDRESS_SQL_COL09 "_REFERENCE"
#define ADDRESS_INVALID_NUMBER 0
#define ADDRESS_MAX_STREET_LENGTH 35
#define ADDRESS_MAX_NUMBER 35
#define ADDRESS_NUMBER_OF_BRAZILIAN_STATES 27
#define ADDRESS_CEP_LENGTH 8
#define ADDRESS_CEP_MASK "99999-999;_"
#define ADDRESS_CEP_LENGTH_WITH_MASK  ADDRESS_CEP_LENGTH + 1

#define CATEGORY_SQL_TABLE_NAME "_CATEGORIES"
#define CATEGORY_SQL_COL01 "_IMAGEID"
#define CATEGORY_SQL_COL02 "_NAME"
#define CATEGORY_MAX_NAME_LENGTH 35

#define IMAGE_SQL_TABLE_NAME "_IMAGES"
#define IMAGE_SQL_COL01 "_NAME"
#define IMAGE_SQL_COL02 "_IMAGE"
#define IMAGE_MAX_NAME_LENGTH 35
#define IMAGE_MAX_SIZE 1048576 //1MB

#define NOTE_SQL_TABLE_NAME "_NOTES"
#define NOTE_SQL_COL01 "_NUMBER"
#define NOTE_SQL_COL02 "_DATE"
#define NOTE_SQL_COL03 "_PERSONID"
#define NOTE_SQL_COL04 "_CASH"
#define NOTE_SQL_COL05 "_OBSERVATION"
#define NOTE_SQL_COL06 "_DISCCOUNT"

#define NOTE_ITEMS_SQL_TABLE_NAME "_NOTE_ITEMS"
#define NOTE_ITEMS_SQL_COL01 "_NOTEID"
#define NOTE_ITEMS_SQL_COL02 "_PRODUCTID"
#define NOTE_ITEMS_SQL_COL03 "_AMMOUNT"
#define NOTE_ITEMS_SQL_COL04 "_PRICE"
#define NOTE_ITEMS_SQL_COL05 "_IS_PACK"
#define NOTE_ITEMS_SQL_COL06 "_PACK_UNITY"
#define NOTE_ITEMS_SQL_COL07 "_PACK_AMMOUNT"
#define NOTE_NUMBER_OF_COLUMNS 5
#define NOTE_MAX_NUMBER_OF_ITEMS 100

#define EMPLOYEE_SQL_TABLE_NAME "_EMPLOYEES"
#define EMPLOYEE_SQL_COL01 "_PERSONID"
#define EMPLOYEE_SQL_COL02 "_PINCODE"
#define EMPLOYEE_SQL_COL03 "_NOTE_EDIT"
#define EMPLOYEE_SQL_COL04 "_NOTE_REMOVE"

#define SUPPLIER_SQL_TABLE_NAME "_SUPPLIERS"
#define SUPPLIER_SQL_COL01 "_PERSONID"

#define PERSON_SQL_TABLE_NAME "_PERSONS"
#define PERSON_SQL_COL01 "_IMAGEID"
#define PERSON_SQL_COL02 "_NAME"
#define PERSON_SQL_COL03 "_ALIAS"
#define PERSON_SQL_COL04 "_EMAIL"
#define PERSON_SQL_COL05 "_CPF_CNPJ"
#define PERSON_SQL_COL06 "_RG_IE"
#define PERSON_SQL_COL07 "_DETAILS"
#define PERSON_SQL_COL08 "_BIRTHDATE"
#define PERSON_SQL_COL09 "_CREATIONDATE"
#define PERSON_SQL_COL10 "_IS_COMPANY"
#define PERSON_MAX_NAME_LENGTH 35
#define PERSON_MAX_DETAILS_LENGTH 35
#define PERSON_FILTER_EMPLOYEE SQL_COLID " IN (SELECT " EMPLOYEE_SQL_COL01 " FROM " EMPLOYEE_SQL_TABLE_NAME ")"
#define PERSON_FILTER_SUPPLIER SQL_COLID " IN (SELECT " SUPPLIER_SQL_COL01 " FROM " SUPPLIER_SQL_TABLE_NAME ")"

#define PHONE_SQL_TABLE_NAME "_PHONES"
#define PHONE_SQL_COL01 "_PERSONID"
#define PHONE_SQL_COL02 "_COUNTRYCODE"
#define PHONE_SQL_COL03 "_CODE"
#define PHONE_SQL_COL04 "_NUMBER"
#define PHONE_SQL_COL05 "_NAME"
#define PHONE_DEFAULT_COUNTRY_CODE_VALUE 55
#define PHONE_DEFAULT_CODE_VALUE 54
#define PHONE_DEFAULT_COUNTRY_CODE_VALUE_STR "55"
#define PHONE_DEFAULT_CODE_VALUE_STR "54"

#define EMPLOYEE_PINCODE_LENGTH 4

#define PRODUCT_SQL_TABLE_NAME "_PRODUCTS"
#define PRODUCT_SQL_COL01 "_NAME"
#define PRODUCT_SQL_COL02 "_CATEGORYID"
#define PRODUCT_SQL_COL03 "_IMAGEID"
#define PRODUCT_SQL_COL04 "_UNITY"
#define PRODUCT_SQL_COL05 "_DETAILS"
#define PRODUCT_SQL_COL06 "_AVAILABLE_AT_NOTES"
#define PRODUCT_SQL_COL07 "_AVAILABLE_AT_SHOP"
#define PRODUCT_SQL_COL08 "_AVAILABLE_TO_BUY"
#define PRODUCT_SQL_COL09 "_AVAILABLE_TO_SELL"

#define PRODUCT_MAX_NAME_LENGTH 35
#define PRODUCT_MAX_UNITY_LENGTH 4
#define PRODUCT_MAX_DETAILS_LENGTH 35
#define PRODUCT_FILTER_NOTE PRODUCT_SQL_TABLE_NAME "." PRODUCT_SQL_COL06 " = TRUE"
#define PRODUCT_FILTER_SHOP PRODUCT_SQL_TABLE_NAME "." PRODUCT_SQL_COL07 " = TRUE"
#define PRODUCT_FILTER_SELL PRODUCT_SQL_TABLE_NAME "." PRODUCT_SQL_COL09 " = TRUE"

#define PRODUCT_BARCODE_SQL_TABLE_NAME "_PRODUCT_BARCODES"
#define PRODUCT_BARCODE_SQL_COL01 "_PRODUCTID"
#define PRODUCT_BARCODE_SQL_COL02 "_CODE"

#define USER_SQL_TABLE_NAME "_USERS"
#define USER_SQL_COL01 "_USER"
#define USER_SQL_COL02 "_PASSWORD"
#define USER_SQL_COL03 "_ACCESS_NOTE"
#define USER_SQL_COL04 "_ACCESS_REMINDER"
#define USER_SQL_COL05 "_ACCESS_CALCULATOR"
#define USER_SQL_COL06 "_ACCESS_SHOP"
#define USER_SQL_COL07 "_ACCESS_USER"
#define USER_SQL_COL08 "_ACCESS_PRODUCT"
#define USER_SQL_COL09 "_ACCESS_SETTINGS"
#define USER_SQL_COL10 "_ACCESS_PERSON"
#define USER_SQL_COL11 "_ACCESS_CATEGORY"
#define USER_SQL_COL12 "_ACCESS_IMAGE"
#define USER_SQL_COL13 "_ACCESS_RESERVATION"
#define USER_SQL_COL14 "_ACCESS_SHOPPING_LIST"
#define USER_SQL_COL15 "_ACCESS_PERSON_EMPLOYEE"
#define USER_SQL_COL16 "_ACCESS_PERSON_SUPPLIER"
#define USER_SQL_COL17 "_ACCESS_PRODUCT_BARCODE"
#define USER_SQL_DEFAULT_NAME "ADMIN"
#define USER_SQL_DEFAULT_PASSWORD "admin"
#define USER_MAX_USERNAME_LENGTH 20
#define USER_MAX_PASSWORD_LENGTH 20

#define REMINDER_SQL_TABLE_NAME "_REMINDERS"
#define REMINDER_SQL_COL01 "_TITLE"
#define REMINDER_SQL_COL02 "_MESSAGE"
#define REMINDER_SQL_COL03 "_FAVORITE"
#define REMINDER_SQL_COL04 "_CAPITALIZATION"
#define REMINDER_SQL_COL05 "_SIZE"
#define REMINDER_SQL_COL06 "_BARCODEHRI"
#define REMINDER_SQL_COL07 "_BARCODE"

#define REMINDER_MAX_BARCODE_CODE93_LENGTH 255

#define SHOPPING_LIST_SQL_TABLE_NAME "_SHOPPING_LISTS"
#define SHOPPING_LIST_SQL_COL01 "_PERSONID"
#define SHOPPING_LIST_SQL_COL02 "_IMAGEID"
#define SHOPPING_LIST_SQL_COL03 "_TITLE"
#define SHOPPING_LIST_SQL_COL04 "_DESCRIPTION"
#define SHOPPING_LIST_SQL_COL05 "_WEEK_DAYS"
#define SHOPPING_LIST_SQL_COL06 "_MONTH_DAYS"

#define SHOPPING_LIST_ITEMS_SQL_TABLE_NAME "_SHOPPING_LIST_ITEMS"
#define SHOPPING_LIST_ITEMS_SQL_COL01 "_SHOPPINGLISTID"
#define SHOPPING_LIST_ITEMS_SQL_COL02 "_PRODUCTID"
#define SHOPPING_LIST_ITEMS_SQL_COL03 "_AMMOUNT"
#define SHOPPING_LIST_ITEMS_SQL_COL04 "_PRICE"
#define SHOPPING_LIST_ITEMS_SQL_COL05 "_IS_PACK"
#define SHOPPING_LIST_ITEMS_SQL_COL06 "_PACK_UNITY"
#define SHOPPING_LIST_ITEMS_SQL_COL07 "_PACK_AMMOUNT"
#define SHOPPING_LIST_ITEMS_SQL_COL08 "_SHOW_AMMOUNT"
#define SHOPPING_LIST_ITEMS_SQL_COL09 "_SHOW_PRICE"

#define SHOPPING_LIST_NUMBER_OF_COLUMNS 4
#define SHOPPING_LIST_MAX_NUMBER_OF_ITEMS 100
#define SHOPPING_LIST_SEPARATOR ";"

#define RESERVATION_SQL_TABLE_NAME "_RESERVATIONS"
#define RESERVATION_SQL_COL01 "_NUMBER"
#define RESERVATION_SQL_COL02 "_NAME"
#define RESERVATION_SQL_COL03 "_LOCATION"
#define RESERVATION_SQL_COL04 "_DATETIME"
#define RESERVATION_SQL_COL05 "_AMMOUNT"
#define RESERVATION_SQL_COL06 "_OBSERVATION"
#define RESERVATION_SQL_COL07 "_PHONE"

#define ACTIVE_USERS_SQL_TABLE_NAME "_ACTIVE_USERS"
#define ACTIVE_USERS_SQL_COL01 "_POSTGRE_PID"
#define ACTIVE_USERS_SQL_COL02 "_NAME"
#define ACTIVE_USERS_SQL_COL03 "_MACHINE"
#define ACTIVE_USERS_SQL_COL04 "_LOGIN"

#define DISCOUNT_SQL_TABLE_NAME "_DISCOUNTS"
#define DISCOUNT_SQL_COL01 "_CODE"
#define DISCOUNT_SQL_COL02 "_EXPIRES"
#define DISCOUNT_SQL_COL03 "_EXPDATE"
#define DISCOUNT_SQL_COL04 "_TYPE"
#define DISCOUNT_SQL_COL05 "_VALUE"
#define DISCOUNT_SQL_COL06 "_PERCENTAGE"
#define DISCOUNT_SQL_COL07 "_REDEEMED"
#define DISCOUNT_SQL_COL08 "_DESCRIPTION"

#define DISCOUNT_ITEMS_SQL_TABLE_NAME "_DISCOUNT_ITEMS"
#define DISCOUNT_ITEMS_SQL_COL01 "_DISCOUNTID"
#define DISCOUNT_ITEMS_SQL_COL02 "_PRODUCTID"
#define DISCOUNT_ITEMS_SQL_COL03 "_AMMOUNT"

#define DISCOUNT_CODE_MIN_LENGTH 4
#define DISCOUNT_CODE_MAX_LENGTH 20

#endif // DEFINES_H
