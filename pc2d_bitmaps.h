#define go_width 31
#define go_height 31
static unsigned char go_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00,
   0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00,
   0x00, 0xff, 0x07, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xff, 0x1f, 0x00,
   0x00, 0xff, 0x3f, 0x00, 0x00, 0xff, 0x1f, 0x00, 0x00, 0xff, 0x0f, 0x00,
   0x00, 0xff, 0x07, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00,
   0x00, 0x3f, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
   0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define go1_width 31
#define go1_height 31
static unsigned char go1_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00,
   0x00, 0x03, 0x40, 0x00, 0x00, 0x07, 0x40, 0x00, 0x00, 0x1f, 0x40, 0x00,
   0x00, 0x3f, 0x40, 0x00, 0x00, 0xff, 0x40, 0x00, 0x00, 0xff, 0x41, 0x00,
   0x00, 0xff, 0x47, 0x00, 0x00, 0xff, 0x4f, 0x00, 0x00, 0xff, 0x5f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x5f, 0x00, 0x00, 0xff, 0x4f, 0x00,
   0x00, 0xff, 0x47, 0x00, 0x00, 0xff, 0x41, 0x00, 0x00, 0xff, 0x40, 0x00,
   0x00, 0x3f, 0x40, 0x00, 0x00, 0x1f, 0x40, 0x00, 0x00, 0x07, 0x40, 0x00,
   0x00, 0x03, 0x40, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define stop_width 31
#define stop_height 31
static unsigned char stop_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define resdn_width 31
#define resdn_height 31
static unsigned char resdn_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xe0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x01, 0x80, 0xff, 0xff, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xfe, 0x3f, 0x00,
   0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xf8, 0x0f, 0x00,
   0x00, 0xf0, 0x07, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0xe0, 0x03, 0x00,
   0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define resup_width 31
#define resup_height 31
static unsigned char resup_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00,
   0x00, 0xf0, 0x07, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0xf8, 0x0f, 0x00,
   0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfe, 0x3f, 0x00,
   0x00, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x80, 0xff, 0xff, 0x00,
   0xc0, 0xff, 0xff, 0x01, 0xe0, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define pause_width 31
#define pause_height 31
static unsigned char pause_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00,
   0x00, 0x07, 0x70, 0x00, 0x00, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define back_width 31
#define back_height 31
static unsigned char back_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20, 0x00,
   0x80, 0x00, 0x30, 0x00, 0x80, 0x00, 0x38, 0x00, 0x80, 0x00, 0x3e, 0x00,
   0x80, 0x00, 0x3f, 0x00, 0x80, 0xc0, 0x3f, 0x00, 0x80, 0xe0, 0x3f, 0x00,
   0x80, 0xf8, 0x3f, 0x00, 0x80, 0xfc, 0x3f, 0x00, 0x80, 0xfe, 0x3f, 0x00,
   0x80, 0xff, 0x3f, 0x00, 0x80, 0xfe, 0x3f, 0x00, 0x80, 0xfc, 0x3f, 0x00,
   0x80, 0xf8, 0x3f, 0x00, 0x80, 0xe0, 0x3f, 0x00, 0x80, 0xc0, 0x3f, 0x00,
   0x80, 0x00, 0x3f, 0x00, 0x80, 0x00, 0x3e, 0x00, 0x80, 0x00, 0x38, 0x00,
   0x80, 0x00, 0x30, 0x00, 0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00};

#define zoom_width 16
#define zoom_height 16
#define zoom_x_hot 5
#define zoom_y_hot 5
static unsigned char zoom_bits[] = {
   0x00, 0x00, 0xf8, 0x00, 0x04, 0x01, 0x02, 0x02, 0x02, 0x02, 0x22, 0x02,
   0x02, 0x02, 0x02, 0x02, 0x04, 0x01, 0xf8, 0x02, 0x00, 0x04, 0x00, 0x08,
   0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x00};

#define roam_width 16
#define roam_height 16
#define roam_x_hot 7
#define roam_y_hot 7
static unsigned char roam_bits[] = {
   0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0x80, 0x00, 0x80, 0x00, 0x84, 0x20,
   0x86, 0x60, 0xff, 0xff, 0x86, 0x60, 0x84, 0x20, 0x80, 0x00, 0x80, 0x00,
   0x80, 0x00, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00};

#define stretch_width 16
#define stretch_height 16
#define stretch_x_hot 7
#define stretch_y_hot 7
static unsigned char stretch_bits[] = {
   0xe0, 0x07, 0xf0, 0x08, 0xfc, 0x30, 0xfc, 0x20, 0xfe, 0x40, 0xff, 0x80,
   0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xff, 0x80, 0xfe, 0x40,
   0xfc, 0x20, 0xfc, 0x30, 0xf0, 0x08, 0xe0, 0x07};

#define track_width 16
#define track_height 16
static unsigned char track_bits[] = {
   0x00, 0x00, 0x40, 0x04, 0xe0, 0x0e, 0xe4, 0x0e, 0xee, 0x0e, 0x0e, 0x20,
   0x0e, 0x73, 0xe0, 0x77, 0xf0, 0x77, 0xf8, 0x07, 0xf8, 0x07, 0xf8, 0x07,
   0xf0, 0x03, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00};

#define world_width 16
#define world_height 16
static unsigned char world_bits[] = {
   0x00, 0x00, 0xe0, 0x03, 0x18, 0x0c, 0x64, 0x11, 0xf6, 0x23, 0xfe, 0x27,
   0xe1, 0x47, 0x83, 0x65, 0x03, 0x61, 0x03, 0x73, 0xc1, 0x73, 0xc2, 0x33,
   0xc2, 0x31, 0x84, 0x10, 0x18, 0x0c, 0xe0, 0x03};

#define text_width 16
#define text_height 16
#define text_x_hot 14
#define text_y_hot 14
static unsigned char text_bits[] = {
   0x00, 0x00, 0xf0, 0x7f, 0x28, 0x40, 0x24, 0x40, 0x22, 0x40, 0x3e, 0x40,
   0x02, 0x40, 0xfa, 0x5f, 0x02, 0x40, 0xfa, 0x5f, 0x02, 0x40, 0xfa, 0x5f,
   0x02, 0x40, 0x02, 0x40, 0xfe, 0x7f, 0x00, 0x00};

#define md_width 16
#define md_height 16
static unsigned char md_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x3e, 0xc6, 0x22,
   0xaa, 0x42, 0x92, 0x42, 0x92, 0x42, 0x82, 0x42, 0x82, 0x42, 0x82, 0x22,
   0x82, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define watch_width 17
#define watch_height 17
#define watch_x_hot 8
#define watch_y_hot 8
static unsigned char watch_bits[] = {
   0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0xc0, 0x07, 0x00, 0x30, 0x18, 0x00,
   0x08, 0x21, 0x00, 0x08, 0x21, 0x00, 0x04, 0x41, 0x00, 0x04, 0x41, 0x00,
   0x04, 0x41, 0x00, 0x04, 0x42, 0x00, 0x04, 0x44, 0x00, 0x08, 0x20, 0x00,
   0x08, 0x20, 0x00, 0x30, 0x18, 0x00, 0xc0, 0x07, 0x00, 0x80, 0x03, 0x00,
   0x80, 0x03, 0x00};

#define watch_mask_width 17
#define watch_mask_height 17
#define watch_mask_x_hot 8
#define watch_mask_y_hot 8
static unsigned char watch_mask_bits[] = {
   0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00, 0xf0, 0x1f, 0x00, 0xf8, 0x3f, 0x00,
   0xfc, 0x7f, 0x00, 0xfc, 0x7f, 0x00, 0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00,
   0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00, 0xfc, 0x7f, 0x00,
   0xfc, 0x7f, 0x00, 0xf8, 0x3f, 0x00, 0xf0, 0x1f, 0x00, 0xc0, 0x07, 0x00,
   0xc0, 0x07, 0x00};

#define measure_width 16
#define measure_height 16
#define measure_x_hot 6
#define measure_y_hot 7
static unsigned char measure_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xfe, 0x7f, 0x02, 0x48, 0x02, 0x08, 0x02, 0x08,
   0x02, 0x08, 0x42, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x02, 0x08,
   0x02, 0x08, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0x00};

