//-----------------------------------------------------------------------------
//Au clair de la lune
//-----------------------------------------------------------------------------
{
ase::sol::music_c oMusic;
oMusic.beat_per_minute = 115;
oMusic.duree = EDuree::eNoire;
oMusic.left_alterations.add(note_c(EHauteur::eFa, EAlteration::eDiese));
//oMusic.left_notes.add(note_c(ESilence::eBaton));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eBlanche, EPointee::eTrue));
oMusic.left_notes.add(note_c(ESilence::eSoupir));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eBlanche, EPointee::eTrue));
oMusic.left_notes.add(note_c(ESilence::eSoupir));

oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eBlanche, EPointee::eTrue));
oMusic.left_notes.add(note_c(ESilence::eSoupir));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eBlanche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eBlanche, EPointee::eTrue));
oMusic.left_notes.add(note_c(ESilence::eSoupir));

EMP_CALL_OR_DIE(pGame->emphase().create_music(
    utf32_string("au_clair_de_la_lune"),
    utf32_string("Au clair de la lune"),
    oMusic,
    &pGame->emphase().mapping()));
}

//-----------------------------------------------------------------------------
// Greensleeves
//-----------------------------------------------------------------------------
{
ase::sol::music_c oMusic;
oMusic.beat_per_minute = 120;
oMusic.duree = EDuree::eNoire;
oMusic.left_alterations.add(note_c(EHauteur::eSi, EAlteration::eBemol));
//oMusic.left_notes.add(note_c(ESilence::eBaton));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));

oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eBlanche));

oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eNoire, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e1, EDuree::eNoire, EPointee::eTrue));

oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eNoire));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eCroche, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EAlteration::eDiese, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eBlanche, EPointee::eTrue));

EMP_CALL_OR_DIE(pGame->emphase().create_music(
    utf32_string("greensleeves"),
    utf32_string("Greensleeves"),
    oMusic,
    &pGame->emphase().mapping()));
}

//-----------------------------------------------------------------------------
// Mario bros
//-----------------------------------------------------------------------------
{
ase::sol::music_c oMusic;
oMusic.beat_per_minute = 100;
oMusic.duree = EDuree::eNoire;
//oMusic.left_notes.add(note_c(ESilence::eBaton));

oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eNoire, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eNoire, EPointee::eTrue));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EAlteration::eBemol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EAlteration::eBemol, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e1, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSi, EOctave::e2, EDuree::eDoubleCroche));

oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EAlteration::eBemol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EAlteration::eDiese, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EAlteration::eDiese, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eLa, EOctave::e1, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eSol, EAlteration::eBemol, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eFa, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eRe, EAlteration::eDiese, EOctave::e2, EDuree::eCroche));
oMusic.left_notes.add(note_c(EHauteur::eMi, EOctave::e2, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e3, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e3, EDuree::eDoubleCroche));
oMusic.left_notes.add(note_c(EHauteur::eDo, EOctave::e3, EDuree::eCroche, EPointee::eTrue));

EMP_CALL_OR_DIE(pGame->emphase().create_music(
    utf32_string("mario_bros"),
    utf32_string("Mario Bros"),
    oMusic,
    &pGame->emphase().mapping()));
}



