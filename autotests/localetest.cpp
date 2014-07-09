#include <QtTest>
#include <QtCore>

#include "../src/l10n_language.h"
#include "../src/l10n_locale.h"

class localeTest : public QObject
{
    Q_OBJECT
private slots:
    void testSimpleCtor();
    void testVariantCtor();
    void testKdeLocaleStringCtor();
    void testComplexList();
    void testEnUsComplexityException();
    void testWriteFile();
    void testWriteFileWithInvalidLocale();
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
    // Only one language with variant. (ca_ES@valencia)
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

void localeTest::testComplexList()
{
    // All languages use everything.
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("zh_CN@valencia")));
    list.append(new Kubuntu::Language(QLatin1String("zh_TW@latin")));
    list.append(new Kubuntu::Language(QLatin1String("zh")));
    list.append(new Kubuntu::Language(QLatin1String("ca_ES@yolo")));
    list.append(new Kubuntu::Language(QLatin1String("es")));
    list.append(new Kubuntu::Language(QLatin1String("de_AT@servas")));
    list.append(new Kubuntu::Language(QLatin1String("de")));

    Kubuntu::Locale l(list, QLatin1String("US"));
    QCOMPARE(l.systemLocaleString(), QLatin1String("zh_CN.UTF-8@valencia"));
    QCOMPARE(l.systemLanguagesString(), QLatin1String("zh:ca:es:de:en"));
}

void localeTest::testEnUsComplexityException()
{
    // en_US is the only kde language that must not override the manually
    // defined country. The reason is that en_US is the uniform system
    // base language, so en_* always results in a valid english localization
    // that is somewhat uniform and useful to everyone. In such a setup, the
    // country must be preserved as otherwise en_CA en_AU and the likes
    // end up with american numeric/currency/time values even when they should
    // not apply.
    // This particularly is in stark contrast to zh_TW and zh_CN who
    // respectively use different signs to say things, so there is no
    // uniform zh that would make sense regardless.
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("en_US@valencia")));
    Kubuntu::Locale l(list, QLatin1String("AU"));
    QCOMPARE(l.systemLocaleString(), QLatin1String("en_AU.UTF-8@valencia"));
    QCOMPARE(l.systemLanguagesString(), QLatin1String("en"));
}

void localeTest::testWriteFile()
{
    QTemporaryFile temp;
    QVERIFY2(temp.open(), "opening temporary file failed");
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("en_US@valencia")));
    list.append(new Kubuntu::Language(QLatin1String("en_US")));
    list.append(new Kubuntu::Language(QLatin1String("en_GB")));
    list.append(new Kubuntu::Language(QLatin1String("de")));
    list.append(new Kubuntu::Language(QLatin1String("fr")));
    list.append(new Kubuntu::Language(QLatin1String("en_US")));
    Kubuntu::Locale l(list, QLatin1String("US"));
    QVERIFY2(l.writeToFile(temp.fileName()), "writing locale to file failed!");

    // Read. Validate LANG, LANGUAGE and first as well as last LC value.
    QByteArray line;
    QVERIFY(temp.reset());
    QVERIFY(!temp.atEnd());

    line = temp.readLine();
    QCOMPARE(line, QByteArray("export LANGUAGE=en:de:fr:en\n"));
    QVERIFY(!temp.atEnd());

    line = temp.readLine();
    QCOMPARE(line, QByteArray("export LANG=en_US.UTF-8@valencia\n"));
    QVERIFY(!temp.atEnd());

    line = temp.readLine();
    QCOMPARE(line, QByteArray("export LC_NUMERIC=en_US.UTF-8@valencia\n"));
    // Must contain time, monetary, paper, identification, name, address, telephone.
    for (int i = 0; i < 7; ++i) {
        temp.readLine();
        QVERIFY(!temp.atEnd());
    }

    line = temp.readLine();
    QCOMPARE(line, QByteArray("export LC_MEASUREMENT=en_US.UTF-8@valencia\n"));
    QVERIFY(temp.atEnd());
}

void localeTest::testWriteFileWithInvalidLocale()
{
    QTemporaryFile temp;
    QVERIFY2(temp.open(), "opening temporary file failed");
    LangPtrList list;
    list.append(new Kubuntu::Language(QLatin1String("en_US")));
    list.append(new Kubuntu::Language(QLatin1String("fr")));
    Kubuntu::Locale l(list, QLatin1String("BE"));
    QVERIFY2(l.writeToFile(temp.fileName()), "writing locale to file failed!");

    // Read. Validate LANG, LANGUAGE and first as well as last LC value.
    QByteArray line;
    QVERIFY(temp.reset());
    QVERIFY(!temp.atEnd());

    line = temp.readLine();
    QCOMPARE(line, QByteArray("export LANGUAGE=en:fr:en\n"));
    QVERIFY(temp.atEnd());
}

QTEST_MAIN(localeTest)

#include "localetest.moc"
