/**
 * Ƕ��ʽϵͳ���Լ��� main ����ʱ���붨�屾�ꡣ
 *
 */
#define USE_GUI_MAIN 1

/**
 * �����Ҫ֧��Ԥ�Ƚ����λͼ���壬�붨�屾�ꡣһ��ֻ�� RAM ��Сʱ�������ñ��ꡣ
 * #define WITH_BITMAP_FONT 1
 */

/**
 * ���֧�� png/jpeg ͼƬ���붨�屾��
 *
 * #define WITH_STB_IMAGE 1
 */

/**
 * ����� stb ֧�� Truetype ���壬�붨�屾��
 *
 */
#define WITH_STB_FONT 1

/**
 * ����� freetype ֧�� Truetype ���壬�붨�屾�ꡣ
 *
 * #define WITH_FT_FONT 1
 */

/**
 * ���֧�ִ��ļ�ϵͳ������Դ���붨�屾��
 *
 * #define WITH_FS_RES 1
 */

/**
 * ������屾�꣬ʹ�ñ�׼�� UNICODE �����㷨��������Դ��Ϊ���ޣ��붨�屾�ꡣ
 *
 * #define WITH_UNICODE_BREAK 1
 */

/**
 * ������屾�꣬��ͼƬ����� BGRA8888 ��ʽ���������� RGBA8888 �ĸ�ʽ��
 * ��Ӳ���� 2D ������Ҫ BGRA ��ʽʱ�������ñ��ꡣ
 *
 * #define WITH_BITMAP_BGRA 1
 */

/**
 * ������屾�꣬����͸���� PNG ͼƬ����� BGR565 ��ʽ�����鶨�塣
 * ����� LCD �ĸ�ʽ����һ�£����Դ����������ܡ�
 *
 * #define WITH_BITMAP_BGR565 1
 */

/**
 * ����������뷨���붨�屾��
 *
 * #define WITH_NULL_IM 1
 */

/**
 * ����б�׼�� malloc/free/calloc �Ⱥ������붨�屾��
 *
 */
#define HAS_STD_MALLOC 1

/**
 * ����б�׼�� fopen/fclose �Ⱥ������붨�屾��
 *
 * #define HAS_STDIO 1
 */

/**
 * ����б�׼�� pthread �Ⱥ������붨�屾��
 *
 * #define HAS_PTHREAD 1
 */

/**
 * ������Ż��汾�� memcpy �������붨�屾��
 *
 * #define HAS_FAST_MEMCPY 1
 */

/**
 * ������� wcsxxx ֮��ĺ���û�ж���ʱ���붨��ú�
 *
 * #define WITH_WCSXXX 1
 */

/**
 * ������� STM32 G2D Ӳ�����٣��붨�屾��
 *
 * #define WITH_STM32_G2D 1
 */

/**
 * ������� NXP PXP Ӳ�����٣��붨�屾��
 *
 * #define WITH_PXP_G2D 1
 */

/**
 * ��û�� GPU ʱ��������� agge ��Ϊ nanovg �ĺ�ˣ��� agg ��Ϊ��ˣ�С���죬ͼ�������Բ���붨�屾�ꡣ
 *
 * #define WITH_NANOVG_AGGE 1
 */

/**
 * ��û�� GPU ʱ��������� agg ��Ϊ nanovg �ĺ�ˣ��� agge ��Ϊ��ˣ�������ͼ�������ã����붨�屾�ꡣ
 * ע�⣺agg ���� GPL Э�鿪Դ��
 *
 * #define WITH_NANOVG_AGG 1
 */

/**
 * ����������ָ�룬�붨�屾��
 *
 * #define ENABLE_CURSOR 1
 */

/**
 * ���ڵͶ�ƽ̨�������ʹ�ÿؼ��������붨�屾�ꡣ
 *
 */
#define WITHOUT_WIDGET_ANIMATORS 1

/**
 * ���ڵͶ�ƽ̨�������ʹ�ô��ڶ������붨�屾�ꡣ
 *
 */
#define WITHOUT_WINDOW_ANIMATORS 1

/**
 * ���ڵͶ�ƽ̨�������ʹ�öԻ���������ԣ��붨�屾�ꡣ
 *
 */
#define WITHOUT_DIALOG_HIGHLIGHTER 1

/**
 * ���ڵͶ�ƽ̨�������ʹ����չ�ؼ����붨�屾�ꡣ
 *
 */
#define WITHOUT_EXT_WIDGETS 1

/**
 * ���ڵͶ�ƽ̨������ڴ治�����ṩ������ FrameBuffer���붨�屾�����þֲ� FrameBuffer���ɴ���������Ⱦ���ܡ�
 *
 */
#define FRAGMENT_FRAME_BUFFER_SIZE (800 * 600)
