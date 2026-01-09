#include "datapage.h"

DataPage::DataPage(QTableView* tableView, QWidget *parent) : QWidget(parent) {
    m_tableView = tableView;
    m_model = new QStandardItemModel(this);
    m_tableView->setModel(m_model);
    
    

}

DataPage::~DataPage() {
}
