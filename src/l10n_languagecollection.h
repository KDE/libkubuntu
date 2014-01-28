#ifndef KUBUNTU_L10N_LANGUAGECOLLECTION_H
#define KUBUNTU_L10N_LANGUAGECOLLECTION_H

#include "export.h"

#include <QObject>

namespace Kubuntu {

class Language;

class LanguageCollectionPrivate;
class KUBUNTU_EXPORT LanguageCollection : public QObject
{
    Q_OBJECT
    friend class LanguagePrivate;
public:
    explicit LanguageCollection(QObject *parent = 0);
    bool isUpdated();
    void update();
    QSet<Language *> languages();

signals:
    void updateProgress(int progress);
    void updated();

private:
    const QScopedPointer<LanguageCollectionPrivate> d_ptr;
    Q_DECLARE_PRIVATE(LanguageCollection)
};

} // namespace Kubuntu

#endif // KUBUNTU_L10N_LANGUAGECOLLECTION_H
