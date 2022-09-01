//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef ASE_SOL_MUSIC_H
#define ASE_SOL_MUSIC_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_cnt_vector.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace ase { namespace sol {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name hauteur_e
#define enum_type
#define enum_values\
    enum_value( none )\
    enum_value( Do   )\
    enum_value( Re   )\
    enum_value( Mi   )\
    enum_value( Fa   )\
    enum_value( Sol  )\
    enum_value( La   )\
    enum_value( Si   )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name octave_e
#define enum_type
#define enum_values\
    enum_value( n0 )\
    enum_value( n1 )\
    enum_value( n2 )\
    enum_value( n3 )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name pointee_e
#define enum_type
#define enum_values\
    enum_value( False )\
    enum_value( True  )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name alteration_e
#define enum_type
#define enum_values\
    enum_value( none        )\
    enum_value( Bemol       )\
    enum_value( DoubleBemol )\
    enum_value( Diese       )\
    enum_value( DoubleDiese )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name duree_e
#define enum_type
#define enum_values\
    enum_value( none            )\
    enum_value( Carree          )\
    enum_value( Ronde           )\
    enum_value( Blanche         )\
    enum_value( Noire           )\
    enum_value( Croche          )\
    enum_value( DoubleCroche    )\
    enum_value( TripleCroche    )\
    enum_value( QuadrupleCroche )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name silence_e
#define enum_values\
    enum_value( none           )\
    enum_value( Baton          )\
    enum_value( Pause          )\
    enum_value( DemiPause      )\
    enum_value( Soupir         )\
    enum_value( DemiSoupir     )\
    enum_value( QuartSoupir    )\
    enum_value( HuitiemeSoupir )\
    enum_value( SeiziemeSoupir )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_(class, note_c)
public:
    hauteur_e hauteur;
    alteration_e alteration;
    duree_e duree;
    octave_e octave;
    pointee_e pointee;

    note_c(void):
        hauteur(),
        alteration(),
        duree(),
        octave(),
        pointee()
    {}

    note_c( c_silence_e a_eSilence):
            hauteur(hauteur_e::none),
            alteration(alteration_e::none),
            duree(static_cast<duree_e>(a_eSilence)), // @@0 enum_cast
            octave(octave_e::n0),
            pointee(pointee_e::False)
    {}

    note_c( c_hauteur_e a_eHauteur,
            c_alteration_e a_eAlteration):
            hauteur(a_eHauteur),
            alteration(a_eAlteration),
            duree(duree_e::none),
            octave(octave_e::n0),
            pointee(pointee_e::False)
    {}

    note_c( c_hauteur_e a_eHauteur,
            c_octave_e a_eOctave,
            c_duree_e a_eDuree):
            hauteur(a_eHauteur),
            alteration(alteration_e::none),
            duree(a_eDuree),
            octave(a_eOctave),
            pointee(pointee_e::False)
    {}

    note_c( c_hauteur_e a_eHauteur,
            c_octave_e a_eOctave,
            c_duree_e a_eDuree,
            c_pointee_e a_ePointee):
            hauteur(a_eHauteur),
            alteration(alteration_e::none),
            duree(a_eDuree),
            octave(a_eOctave),
            pointee(a_ePointee)
    {}

    note_c( c_hauteur_e a_eHauteur,
            c_alteration_e a_eAlteration,
            c_octave_e a_eOctave,
            c_duree_e a_eDuree):
            hauteur(a_eHauteur),
            alteration(a_eAlteration),
            duree(a_eDuree),
            octave(a_eOctave),
            pointee(pointee_e::False)
    {}

    note_c( c_hauteur_e a_eHauteur,
            c_alteration_e a_eAlteration,
            c_octave_e a_eOctave,
            c_duree_e a_eDuree,
            c_pointee_e a_ePointee):
            hauteur(a_eHauteur),
            alteration(a_eAlteration),
            duree(a_eDuree),
            octave(a_eOctave),
            pointee(a_ePointee)
    {}

    note_c(note_c const& a_rFrom):
        hauteur(a_rFrom.hauteur),
        alteration(a_rFrom.alteration),
        duree(a_rFrom.duree),
        octave(a_rFrom.octave),
        pointee(a_rFrom.pointee)
    {}

    note_c& operator=(note_c const& a_rFrom)
    {
        hauteur = a_rFrom.hauteur;
        alteration = a_rFrom.alteration;
        duree = a_rFrom.duree;
        octave = a_rFrom.octave;
        pointee = a_rFrom.pointee;
        return *this;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, music_c)
public:
    float beat_per_minute;
    float beat_per_mesure;
    duree_e duree;
    emp::cnt::vector<note_c> left_notes;
    emp::cnt::vector<note_c> left_alterations;
    emp::cnt::vector<note_c> right_notes;
    emp::cnt::vector<note_c> right_alterations;

    music_c(void):
        beat_per_minute(),
        beat_per_mesure(),
        duree(),
        left_notes(),
        left_alterations(),
        right_notes(),
        right_alterations()
    {}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

