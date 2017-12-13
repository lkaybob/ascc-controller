#define SVNSEGDRV "/dev/7segdrv"
#define KMDRV   "/dev/kmsw"
#define OLEDDRV "/dev/cnoled"
#define TLCDDRV "/dev/cntlcd"
#define DMDRV   "/dev/dmsw"
#define BUZZDRV   "/dev/cnbuzzer"
#define DIPSWDRV "/dev/dipsw"

int keyMatrixInput(int fd, int buzzerFd);
void startLectureBuzzer(int fd);
int processJson(char* original, jsmntok_t t[], int len, char* key, char* value);
static int jsoneq(const char *json, jsmntok_t *tok, const char *s);