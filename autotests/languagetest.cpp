#include <QtTest>
#include <QtCore>

#include "../src/l10n_language.h"

class languageTest : public QObject
{
    Q_OBJECT
private slots:
    void testVariantStripping();
    void testCountryStripping();
};

void languageTest::testVariantStripping()
{
    Kubuntu::Language l(QLatin1String("ca@valencia"));
    QCOMPARE(l.languageCode(), QString("ca@valencia"));
    QCOMPARE(l.systemLanguageCode(), QString("ca"));
}

void languageTest::testCountryStripping()
{
    Kubuntu::Language l(QLatin1String("en_GB"));
    QCOMPARE(l.languageCode(), QString("en_GB"));
    QCOMPARE(l.systemLanguageCode(), QString("en"));
}

#warning how to test the silly package support nonesense?

QTEST_MAIN(languageTest)

#include "languagetest.moc"
