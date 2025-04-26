#include <vm.h>
#ifdef WITH_RAYLIB
#include <raylib.h>

// Désactiver les logs raylib dès le départ
void f_DisableLogs() { SetTraceLogLevel(LOG_NONE); }

// Ouvre une fenêtre (width, height)
void f_InitWindow(VM* vm) {
    double height = POP_DOUBLE(vm);
    double width = POP_DOUBLE(vm);
    InitWindow((int)width, (int)height, "Hello, World!");
}

// Commence un nouveau dessin
void f_BeginDrawing() { BeginDrawing(); }

// Efface l'écran avec une couleur (r, g, b)
void f_ClearBackground(VM* vm) {
    double b = POP_DOUBLE(vm);
    double g = POP_DOUBLE(vm);
    double r = POP_DOUBLE(vm);
    ClearBackground(
        (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, 255});
}

// Dessine un rectangle (x, y, width, height, r, g, b)
void f_DrawRectangle(VM* vm) {
    double b = POP_DOUBLE(vm);
    double g = POP_DOUBLE(vm);
    double r = POP_DOUBLE(vm);
    double height = POP_DOUBLE(vm);
    double width = POP_DOUBLE(vm);
    double y = POP_DOUBLE(vm);
    double x = POP_DOUBLE(vm);

    DrawRectangle(
        (int)x, (int)y, (int)width, (int)height,
        (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, 255});
}

// Termine le dessin courant
void f_EndDrawing() { EndDrawing(); }

// Vérifie si la fenêtre doit se fermer, retourne 1.0 ou 0.0
void f_WindowShouldClose(VM* vm) {
    bool shouldClose = WindowShouldClose();
    PUSH_BOOL(vm, (shouldClose ? 1.0 : 0.0));
}

void f_RightPressed(VM* vm) {
    PUSH_BOOL(vm, IsKeyDown(KEY_RIGHT));
}
void f_LeftPressed(VM* vm) {
    PUSH_BOOL(vm, IsKeyDown(KEY_LEFT));
}
void f_UpPressed(VM* vm) {
    PUSH_BOOL(vm, IsKeyDown(KEY_UP));
}
void f_DownPressed(VM* vm) {
    PUSH_BOOL(vm, IsKeyDown(KEY_DOWN));
}

void f_nop() {}

// Ferme la fenêtre
void f_CloseWindow() { CloseWindow(); }
#endif
// Fonction pour enregistrer tous les bindings
void bind_raylib(VM* vm) {
    #ifdef WITH_RAYLIB
    add_ffi_func(vm, sv_from_cstr("disable-logs"), f_DisableLogs);
    add_ffi_func(vm, sv_from_cstr("init-window"), f_InitWindow);
    add_ffi_func(vm, sv_from_cstr("begin-drawing"), f_BeginDrawing);
    add_ffi_func(vm, sv_from_cstr("clear"), f_ClearBackground);
    add_ffi_func(vm, sv_from_cstr("rect"), f_DrawRectangle);
    add_ffi_func(vm, sv_from_cstr("end-drawing"), f_EndDrawing);
    add_ffi_func(vm, sv_from_cstr("window-should-close"), f_WindowShouldClose);
    add_ffi_func(vm, sv_from_cstr("close-window"), f_CloseWindow);
    add_ffi_func(vm, sv_from_cstr("right-pressed"), f_RightPressed);
    add_ffi_func(vm, sv_from_cstr("down-pressed"), f_DownPressed);
    add_ffi_func(vm, sv_from_cstr("up-pressed"), f_UpPressed);
    add_ffi_func(vm, sv_from_cstr("left-pressed"), f_LeftPressed);
    add_ffi_func(vm, sv_from_cstr("nop"), f_nop);
    #else
    (void)vm;
    #endif
}
