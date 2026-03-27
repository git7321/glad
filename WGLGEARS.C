#include <assert.h>
#include <windows.h>
#include <glad/glad.h>
#include <glad/glad_wgl.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

static HDC hDC;
static HGLRC hRC;
static HWND hWnd;
static HINSTANCE hInst;
static RECT winrect;

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

static GLboolean fullscreen = GL_FALSE;
static GLint samples = 0;
static GLboolean use_srgb = GL_FALSE;
static GLboolean animate = GL_TRUE;

static void
usage(void)
{
   printf("Usage:\n");
   printf("  -srgb             run in sRGB mode\n");
   printf("  -samples N        run in multisample mode with at least N samples\n");
   printf("  -fullscreen       run in fullscreen mode\n");
   printf("  -info             display OpenGL renderer info\n");
   printf("  -geometry WxH+X+Y window geometry\n");
}

static double
current_time(void)
{
   return timeGetTime() / 1000.0;
}

static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
     GLint teeth, GLfloat tooth_depth)
{
   GLint i;
   GLfloat r0, r1, r2;
   GLfloat angle, da;
   GLfloat u, v, len;

   r0 = inner_radius;
   r1 = outer_radius - tooth_depth / 2.0f;
   r2 = outer_radius + tooth_depth / 2.0f;

   da = (float)(2.0 * M_PI / teeth / 4.0);

   glShadeModel(GL_FLAT);
   glNormal3f(0.0, 0.0, 1.0);

   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = (float)(i * 2.0f * M_PI / teeth);
      glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
      if (i < teeth) {
	 glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
	 glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		    width * 0.5f);
      }
   }
   glEnd();

   glBegin(GL_QUADS);
   da = (float)(2.0 * M_PI / teeth / 4.0);
   for (i = 0; i < teeth; i++) {
      angle = (float)(i * 2.0 * M_PI / teeth);

      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da),
		 width * 0.5f);
      glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		 width * 0.5f);
   }
   glEnd();
   glNormal3f(0.0, 0.0, -1.0);

   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = (float)(i * 2.0 * M_PI / teeth);
      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
      glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
      if (i < teeth) {
	 glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		    -width * 0.5f);
	 glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
      }
   }
   glEnd();

   glBegin(GL_QUADS);
   da = (float)(2.0 * M_PI / teeth / 4.0);
   for (i = 0; i < teeth; i++) {
      angle = (float)(i * 2.0 * M_PI / teeth);

      glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		 -width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da),
		 -width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), -width * 0.5f);
      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
   }
   glEnd();

   glBegin(GL_QUAD_STRIP);
   for (i = 0; i < teeth; i++) {
      angle = (float)(i * 2.0 * M_PI / teeth);

      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), width * 0.5f);
      glVertex3f(r1 * (float)cos(angle), r1 * (float)sin(angle), -width * 0.5f);
      u = r2 * (float)cos(angle + da) - r1 * (float)cos(angle);
      v = r2 * (float)sin(angle + da) - r1 * (float)sin(angle);
      len = (float)sqrt(u * u + v * v);
      u /= len;
      v /= len;
      glNormal3f(v, -u, 0.0);
      glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + da), r2 * (float)sin(angle + da), -width * 0.5f);
      glNormal3f((float)cos(angle), (float)sin(angle), 0.0);
      glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da),
		 width * 0.5f);
      glVertex3f(r2 * (float)cos(angle + 2 * da), r2 * (float)sin(angle + 2 * da),
		 -width * 0.5f);
      u = r1 * (float)cos(angle + 3 * da) - r2 * (float)cos(angle + 2 * da);
      v = r1 * (float)sin(angle + 3 * da) - r2 * (float)sin(angle + 2 * da);
      glNormal3f(v, -u, 0.0);
      glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		 width * 0.5f);
      glVertex3f(r1 * (float)cos(angle + 3 * da), r1 * (float)sin(angle + 3 * da),
		 -width * 0.5f);
      glNormal3f((float)cos(angle), (float)sin(angle), 0.0);
   }

   glVertex3f(r1 * (float)cos(0), r1 * (float)sin(0), width * 0.5f);
   glVertex3f(r1 * (float)cos(0), r1 * (float)sin(0), -width * 0.5f);

   glEnd();
   glShadeModel(GL_SMOOTH);

   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= teeth; i++) {
      angle = (float)(i * 2.0 * M_PI / teeth);
      glNormal3f(-(float)cos(angle), -(float)sin(angle), 0.0);
      glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), -width * 0.5f);
      glVertex3f(r0 * (float)cos(angle), r0 * (float)sin(angle), width * 0.5f);
   }
   glEnd();
}

static void
draw(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   glRotatef(view_rotx, 1.0, 0.0, 0.0);
   glRotatef(view_roty, 0.0, 1.0, 0.0);
   glRotatef(view_rotz, 0.0, 0.0, 1.0);

   glPushMatrix();
   glTranslatef(-3.0, -2.0, 0.0);
   glRotatef(angle, 0.0, 0.0, 1.0);
   glCallList(gear1);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(3.1f, -2.0f, 0.0);
   glRotatef(-2.0f * angle - 9.0f, 0.0, 0.0, 1.0);
   glCallList(gear2);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-3.1f, 4.2f, 0.0);
   glRotatef(-2.0f * angle - 25.0f, 0.0, 0.0, 1.0);
   glCallList(gear3);
   glPopMatrix();
}

static void
reshape(int width, int height)
{
   GLfloat h = (GLfloat) height / (GLfloat) width;

   glViewport(0, 0, (GLint) width, (GLint) height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -40.0);
}

static GLfloat
srgb_to_linear(GLfloat c)
{
   if (c <= 0.04045f)
      return c / 12.92f;
   return (float)pow((c + 0.055f) / 1.055f, 2.4f);
}

static void
init(void)
{
   static GLfloat pos[4] = { 5.0f, 5.0f, 10.0f, 0.0 };
   static GLfloat red[4] = { 0.8f, 0.1f, 0.0, 1.0 };
   static GLfloat green[4] = { 0.0, 0.8f, 0.2f, 1.0 };
   static GLfloat blue[4] = { 0.2f, 0.2f, 1.0, 1.0 };
   int i;

   glLightfv(GL_LIGHT0, GL_POSITION, pos);
   glEnable(GL_CULL_FACE);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   if (use_srgb) {
      for (i = 0; i < 3; ++i) {
         red[i] = srgb_to_linear(red[i]);
         green[i] = srgb_to_linear(green[i]);
         blue[i] = srgb_to_linear(blue[i]);
      }
      glEnable(GL_FRAMEBUFFER_SRGB);
   }

   gear1 = glGenLists(1);
   glNewList(gear1, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
   gear(1.0, 4.0f, 1.0, 20, 0.7f);
   glEndList();

   gear2 = glGenLists(1);
   glNewList(gear2, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
   gear(0.5f, 2.0f, 2.0f, 10, 0.7f);
   glEndList();

   gear3 = glGenLists(1);
   glNewList(gear3, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
   gear(1.3f, 2.0f, 0.5f, 10, 0.7f);
   glEndList();

   glEnable(GL_NORMALIZE);
}

static LRESULT CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg) {
   case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
   case WM_SIZE:
      if (wglGetCurrentContext() != NULL) {
         reshape(LOWORD(lParam), HIWORD(lParam));
      }
      return 0;
   case WM_KEYDOWN:
      if (wParam == VK_LEFT)
         view_roty += 5.0;
      else if (wParam == VK_RIGHT)
         view_roty -= 5.0;
      else if (wParam == VK_UP)
         view_rotx += 5.0;
      else if (wParam == VK_DOWN)
         view_rotx -= 5.0;
      else if (wParam == VK_ESCAPE)
         PostQuitMessage(0);
      else if (wParam == 'A')
         animate = !animate;
      return 0;
   }

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void
make_window(const char *name, int x, int y, int width, int height)
{
   int pixelFormat;
   WNDCLASS wc;
   DWORD dwExStyle, dwStyle;
   static const PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,
      24,
      0, 0, 0, 0, 0, 0,
      0,
      0,
      0,
      0, 0, 0, 0,
      16,
      0,
      0,
      PFD_MAIN_PLANE,
      0,
      0, 0, 0
   };

   winrect.left = (long)0;
   winrect.right = (long)width;
   winrect.top = (long) 0;
   winrect.bottom = (long)height;

   hInst = GetModuleHandle(NULL);
   wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   wc.lpfnWndProc = (WNDPROC)WndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInst;
   wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = name;
   RegisterClass(&wc);

   dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   if (!fullscreen) {
      dwStyle = WS_OVERLAPPEDWINDOW;
      AdjustWindowRectEx(&winrect, dwStyle, FALSE, dwExStyle);
   }
   else {
      dwStyle = WS_POPUP;
   }

   hWnd = CreateWindowEx(dwExStyle, name, name,
                         WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                         x, y,
                         winrect.right - winrect.left,
                         winrect.bottom - winrect.top,
                         NULL, NULL, hInst, NULL);

   if (fullscreen) {
      DEVMODE devmode;
      memset(&devmode, 0, sizeof(DEVMODE));
      devmode.dmSize = sizeof(DEVMODE);
      devmode.dmPelsWidth = width;
      devmode.dmPelsHeight = height;
      devmode.dmBitsPerPel = 24;
      devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
      ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
   }

   hDC = GetDC(hWnd);
   pixelFormat = ChoosePixelFormat(hDC, &pfd);
   if (!pixelFormat)
      goto nopixelformat;

   SetPixelFormat(hDC, pixelFormat, &pfd);
   hRC = wglCreateContext(hDC);
   wglMakeCurrent(hDC, hRC);

   gladLoadWGL(hDC);

   if (use_srgb || samples > 0) {
      int i;
      UINT numFormats;
      PIXELFORMATDESCRIPTOR newPfd;
      static const float float_attribs[] = { 0 };

         int int_attribs[64] = {
         WGL_SUPPORT_OPENGL_ARB, TRUE,
         WGL_DRAW_TO_WINDOW_ARB, TRUE,
         WGL_COLOR_BITS_ARB, 24,
         WGL_DEPTH_BITS_ARB, 24,
         WGL_DOUBLE_BUFFER_ARB, TRUE,
         WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, TRUE,
      };
      assert(GLAD_WGL_ARB_create_context);
      i = 10;

      if (use_srgb) {
         int_attribs[i++] = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
         int_attribs[i++] = TRUE;
      }
      if (samples > 0) {
         int_attribs[i++] = WGL_SAMPLE_BUFFERS_ARB;
         int_attribs[i++] = 1;
         int_attribs[i++] = WGL_SAMPLES_ARB;
         int_attribs[i++] = samples;
      }

      int_attribs[i++] = 0;

      pixelFormat = 0;
      if (!wglChoosePixelFormatARB(hDC, int_attribs, float_attribs, 1,
                                   &pixelFormat, &numFormats) ||
          !numFormats)
         goto nopixelformat;

      DescribePixelFormat(hDC, pixelFormat, sizeof(pfd), &newPfd);

      wglMakeCurrent(hDC, NULL);
      wglDeleteContext(hRC);
      DeleteDC(hDC);

      DestroyWindow(hWnd);
      hWnd = CreateWindowEx(dwExStyle, name, name,
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                            x, y,
                            winrect.right - winrect.left,
                            winrect.bottom - winrect.top,
                            NULL, NULL, hInst, NULL);

      hDC = GetDC(hWnd);
      SetPixelFormat(hDC, pixelFormat, &pfd);
      hRC = wglCreateContext(hDC);
      wglMakeCurrent(hDC, hRC);
   }

   gladLoadGL();

   ShowWindow(hWnd, SW_SHOW);
   SetForegroundWindow(hWnd);
   SetFocus(hWnd);
   return;

nopixelformat:
   printf("Error: couldn't get an RGB, Double-buffered");
   if (samples > 0)
      printf(", Multisample");
   if (use_srgb)
      printf(", sRGB");
   printf(" pixelformat\n");
   exit(1);
}

static void
draw_frame()
{
   static int frames = 0;
   static double tRot0 = -1.0, tRate0 = -1.0;
   double dt, t = current_time();

   if (tRot0 < 0.0)
      tRot0 = t;
   dt = t - tRot0;
   tRot0 = t;

   if (animate) {
      angle += (float)(70.0 * dt);
      if (angle > 3600.0)
         angle -= 3600.0;
   }

   draw();
   SwapBuffers(hDC);

   frames++;

   if (tRate0 < 0.0)
      tRate0 = t;
   if (t - tRate0 >= 5.0) {
      GLfloat seconds = (float)(t - tRate0);
      GLfloat fps = frames / seconds;
      printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
             fps);
      fflush(stdout);
      tRate0 = t;
      frames = 0;
   }
}

static void
query_vsync()
{
   int interval = 0;
   if (GLAD_WGL_EXT_swap_control) {
      interval = wglGetSwapIntervalEXT();
   }

   if (interval > 0) {
      printf("Running synchronized to the vertical refresh.  The framerate should be\n");
      if (interval == 1) {
         printf("approximately the same as the monitor refresh rate.\n");
      }
      else if (interval > 1) {
         printf("approximately 1/%d the monitor refresh rate.\n",
                interval);
      }
   }
}

static void
event_loop(void)
{
   MSG msg;

   TIMECAPS tc;
   timeGetDevCaps(&tc, sizeof(tc));
   timeBeginPeriod(tc.wPeriodMin);

   while(1) {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
         if (msg.message == WM_QUIT) break;;
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }

      draw_frame();
   }

   timeEndPeriod(tc.wPeriodMin);
}

static void
parse_geometry(const char *str, int *x, int *y, unsigned int *w, unsigned int *h)
{
   long tw, th, tx, ty;
   char *end;
   if (*str == '=')
      str++;

   tw = LONG_MAX;
   if (isdigit(*str)) {
      tw = strtol(str, &end, 10);
      if (str == end)
         return;
      str = end;
   }

   th = LONG_MAX;
   if (tolower(*str) == 'x') {
      str++;
      th = strtol(str, &end, 10);
      if (str== end)
         return;
      str = end;
   }

   tx = LONG_MAX;
   if (*str == '+' || *str == '-') {
      tx = strtol(str, &end, 10);
      if (str == end)
         return;
      str = end;
   }

   ty = LONG_MAX;
   if (*str == '+' || *str == '-') {
      ty = strtol(str, &end, 10);
      if (str == end)
         return;
      str = end;
   }

   if (tw < LONG_MAX)
      *w = tw;
   if (th < LONG_MAX)
      *h = th;
   if (tx < INT_MAX)
      *x = tx;
   if (ty < INT_MAX)
      *y = ty;
}

int __cdecl
main(int argc, char *argv[])
{
   unsigned int winWidth = 300, winHeight = 300;
   int x = 0, y = 0;
   int i;
   GLboolean printInfo = GL_FALSE;

   for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-info") == 0) {
         printInfo = GL_TRUE;
      }
      else if (strcmp(argv[i], "-srgb") == 0) {
         use_srgb = GL_TRUE;
      }
      else if (i < argc - 1 && strcmp(argv[i], "-samples") == 0) {
         samples = (GLint)strtod(argv[i + 1], NULL);
         ++i;
      }
      else if (strcmp(argv[i], "-fullscreen") == 0) {
         fullscreen = GL_TRUE;
      }
      else if (strcmp(argv[i], "-geometry") == 0) {
         parse_geometry(argv[i+1], &x, &y, &winWidth, &winHeight);
         i++;
      }
      else {
         usage();
         return -1;
      }
   }

   if (fullscreen) {
       x = 0; y = 0;
       winWidth = GetSystemMetrics(SM_CXSCREEN);
       winHeight = GetSystemMetrics(SM_CYSCREEN);
   }

   make_window("wglgears", x, y, winWidth, winHeight);
   reshape(winWidth, winHeight);
   query_vsync();

   if (printInfo) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
   }

   init();

   event_loop();

   wglMakeCurrent (NULL, NULL);
   wglDeleteContext (hRC);
   ReleaseDC (hWnd, hDC);

   return EXIT_SUCCESS;
}
