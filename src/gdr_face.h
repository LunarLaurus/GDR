// src/gdr_face.h
// GDR procedural goblin face widget
#ifndef GDR_FACE_HDR
#define GDR_FACE_HDR

#define GDR_FACE_W  32
#define GDR_FACE_H  32

typedef enum {
    GFACE_HEALTHY = 0,  // >80% HP: grinning
    GFACE_HURT,         // 60-80%: squinting
    GFACE_DAMAGED,      // 40-60%: scowl + crack
    GFACE_BAD,          // 20-40%: grimace + X eye
    GFACE_CRITICAL,     // <20%: skull-eyes
    GFACE_DEAD,         // dead: pancake
    GFACE_OUCH,         // ouch expression
    GFACE_EVIL,         // evil grin
    GFACE_RAGE,         // rage
    GFACE_GOD,          // god mode: glowing
    GFACE_COUNT
} GDRFaceState;

void ST_DrawGDRFace(int x, int y, GDRFaceState state);
GDRFaceState ST_GetGDRFaceState(void);

#endif // GDR_FACE_HDR
