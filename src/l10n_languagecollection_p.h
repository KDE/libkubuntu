#ifndef L10N_LANGUAGECOLLECTION_P_H
#define L10N_LANGUAGECOLLECTION_P_H

#include <LibQApt/Backend>

namespace Kubuntu {

class LanguageCollectionPrivate
{
public:
    LanguageCollectionPrivate();

    QApt::Backend backend;
};

} // namespace Kubuntu

#endif // L10N_LANGUAGECOLLECTION_P_H
