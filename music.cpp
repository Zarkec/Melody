#include "music.h"

Music::Music(QObject* parent) : QObject(parent)
    , m_id(0), m_name(""), m_author(""), m_album(""), m_url(""), m_picurl(""), m_duration(0)
{

}
