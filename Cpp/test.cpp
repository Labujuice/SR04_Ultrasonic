
#include <cstdio>
#include "SR04_auto.h"
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

int fd;
SR04 USonic_1;

//=====START: COPY from other source===== (due to lazy)
#define TRUE 1
#define FALSE -1

int speed_arr[] = {
    B115200,
    B38400,
    B19200,
    B9600,
    B4800,
    B2400,
    B1200,
    B300,
    B38400,
    B19200,
    B9600,
    B4800,
    B2400,
    B1200,
    B300,
};

int name_arr[] = {
    115200,
    38400,
    19200,
    9600,
    4800,
    2400,
    1200,
    300,
    38400,
    19200,
    9600,
    4800,
    2400,
    1200,
    300,
};

void set_speed(int fd, int speed)
{
    int i;
    int status;

    struct termios Opt;
    tcgetattr(fd, &Opt);

    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0)
                perror("tcsetattr fd1");

            return;
        }
        tcflush(fd, TCIOFLUSH);
    }
}

int set_Parity(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return (FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return (FALSE);
    }

    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK;  /* Enable parity checking */
        options.c_iflag &= ~(ICRNL | IGNCR);
        options.c_lflag &= ~(ICANON | ECHO);
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB; /* Enable parity */
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK; /* Disnable parity checking */
        break;

    case 'S':
    case 's': /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;

    default:
        fprintf(stderr, "Unsupported parity\n");
        return (FALSE);
    }

    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return (FALSE);
    }

    /* Set input parity option */

    if (parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}
//=====END: COPY from other source=====



void uart_receive()
{
    uint8_t read_buffer[32]; /* Buffer to store the data received              */
    int bytes_read = 0;      /* Number of bytes read by the read() system call */
    int i = 0;

    bytes_read = read(fd, &read_buffer, 32); //Read data, In this test code, still exist a problem about data lost. Need to be fixed!
    if (bytes_read <= 0)
        return;

    for (i = 0; i < bytes_read; i++) /*printing only the received characters*/
    {
        bool result = USonic_1.parse(read_buffer[i]);
        // printf("0x%x ", read_buffer[i]);
        if(result)
        {
            printf("dist %d mm\n", USonic_1.distance);
        }
    }

    // printf("\n");
}

int main()
{
    // uart_init();
    fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY | O_SYNC);

    set_speed(fd, 9600);
    set_Parity(fd, 8, 1, 'N');

    // Check for errors
    if (fd < 0)
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }

    while (1)
    {
        uart_receive();
        usleep(20000); // 20 ms
    }

    close(fd);
    return 0;
}