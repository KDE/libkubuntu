#include <QtTest>
#include <QtCore>

#include "../src/language.h"
#include "../src/locale.h"

class localeTest : public QObject
{
    Q_OBJECT
private slots:
    void testSimpleCtor();
    void testVariantCtor();
    void testKdeLocaleStringCtor();
};

typedef QList<Kubuntu::Language *> LangPtrList;

void localeTest::testSimpleCtor()
{
    // Only one language without anything fancy. (de_AT)
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("de")));

    Kubuntu::Locale l(list, QLatin1String("at"));
    QCOMPARE(l.systemLocaleString(), QLatin1String("de_AT.UTF-8"));
    QCOMPARE(l.systemLanguagesString(), QLatin1String("de:en"));
}

void localeTest::testVariantCtor()
{
    // Only one language with variant. (ca_ES @valencia)
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("ca@valencia")));

    Kubuntu::Locale l(list, QLatin1String("ES"));
    QCOMPARE(l.systemLocaleString(), QLatin1String("ca_ES.UTF-8@valencia"));
    QCOMPARE(l.systemLanguagesString(), QLatin1String("ca:en"));
}

void localeTest::testKdeLocaleStringCtor()
{
    // KDE Language is a locale. (zh_CN)
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("zh_CN")));

    Kubuntu::Locale l(list, QLatin1String("AT"));
    QCOMPARE(l.systemLocaleString(), QLatin1String("zh_CN.UTF-8"));
    QCOMPARE(l.systemLanguagesString(), QLatin1String("zh:en"));
}


QTEST_MAIN(localeTest)

#include "localetest.moc"
