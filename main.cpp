#include "src/l10n_language.h"
#include "src/l10n_locale.h"

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

#include <QDebug>
#include <QObject>

static KAboutData aboutData(
        "demo", 0,
        ki18n(""), "0",
        ki18n(""), KAboutData::License_GPL_V2,
        ki18n(""),
        ki18n(""),
        "");

class Dummy : public QObject
{
    Q_OBJECT
public:
    Dummy(Kubuntu::Language *l10n)
        : QObject()
        , m_l10n(l10n)
    {
        connect(m_l10n, SIGNAL(supportCompletionProgress(int)),
                this, SLOT(onCompletionProgress(int)));
        connect(m_l10n, SIGNAL(supportComplete()),
                this, SLOT(onSupportedComplete()));
    }

public slots:
    void onCompletionProgress(int progress)
    {
        qDebug() << "completion" << progress;
    }

    void onSupportedComplete()
    {
        qDebug() << "support completed";
    }

private:
    Kubuntu::Language *m_l10n;
};

int main(int argc, char **argv)
{
    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    Kubuntu::Language l10n;
    Dummy d(&l10n);

    qDebug() << l10n.isSupportComplete();
    l10n.completeSupport();

    Kubuntu::Locale locale;
    qDebug() << locale.systemLocaleString();
    qDebug() << locale.systemLanguages();
    qDebug() << locale.systemLanguagesString();

    return app.exec();
}

#include "main.moc"
