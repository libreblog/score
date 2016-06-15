#include "MessagesPanel.hpp"

#include <QListWidget>
#include <Device/Protocol/DeviceInterface.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
namespace Ossia
{
PanelDelegate::PanelDelegate(const iscore::ApplicationContext& ctx):
    iscore::PanelDelegate{ctx},
    m_widget{new QListWidget}
{
}

QWidget*PanelDelegate::widget()
{
    return m_widget;
}

const iscore::PanelStatus& PanelDelegate::defaultPanelStatus() const
{
    static const iscore::PanelStatus status{
        false,
        Qt::BottomDockWidgetArea,
                0,
                QObject::tr("Messages"),
                QObject::tr("Ctrl+Shift+M")};

    return status;
}

void PanelDelegate::on_modelChanged(
        iscore::MaybeDocument oldm,
        iscore::MaybeDocument newm)
{
    QObject::disconnect(m_inbound);
    QObject::disconnect(m_outbound);

    if(!newm)
        return;

    const auto dark1 = QColor(Qt::darkGray).darker();
    const auto dark2 = dark1.darker(); // almost darker than black
    Device::DeviceList& devices= newm->plugin<Explorer::DeviceDocumentPlugin>().list();
    m_inbound = QObject::connect(&devices, &Device::DeviceList::logInbound,
                                 m_widget, [=] (const QString& str) {
        auto lw = new QListWidgetItem{str};
        lw->setBackgroundColor(dark1);
        m_widget->addItem(lw);
        if(m_widget->count() > 500)
            delete m_widget->takeItem(0);
    }, Qt::QueuedConnection);
    m_outbound = QObject::connect(&devices, &Device::DeviceList::logOutbound,
                                 m_widget, [=] (const QString& str) {
        auto lw = new QListWidgetItem{str};
        lw->setBackgroundColor(dark2);
        m_widget->addItem(lw);
        if(m_widget->count() > 500)
            delete m_widget->takeItem(0);
    }, Qt::QueuedConnection);
}

std::unique_ptr<iscore::PanelDelegate> PanelDelegateFactory::make(
        const iscore::ApplicationContext& ctx)
{
    return std::make_unique<PanelDelegate>(ctx);
}

}
