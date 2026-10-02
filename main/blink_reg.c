#include <stdint.h>   // chỉ để có kiểu uint32_t, có thể bỏ và dùng unsigned int

// ---- Địa chỉ thanh ghi (theo TRM) ----
#define REG(addr)               (*(volatile uint32_t *)(addr))

#define GPIO_BASE               0x60004000
#define GPIO_OUT_W1TS_REG       (GPIO_BASE + 0x0008)   // ghi 1 -> set bit
#define GPIO_OUT_W1TC_REG       (GPIO_BASE + 0x000C)   // ghi 1 -> clear bit
#define GPIO_ENABLE_W1TS_REG    (GPIO_BASE + 0x0024)   // bật output
#define GPIO_FUNC0_OUT_SEL_CFG  (GPIO_BASE + 0x0554)   // + 4*n cho chân n

#define IO_MUX_BASE             0x60009000
#define IO_MUX_GPIO4_REG        (IO_MUX_BASE + 0x14)   // GPIO0 = 0x04, mỗi chân +4

#define MCU_SEL_S               12                     // bit 14:12 chọn chức năng chân
#define SIG_GPIO_OUT_IDX        256                    // tín hiệu "GPIO output" thuần

#define LED_PIN                 4

static void delay(volatile uint32_t n)
{
    while (n--) { __asm__ volatile ("nop"); }
}

void app_main(void)     // IDF bắt buộc có hàm này
{
    // 1. IO_MUX: MCU_SEL = 1 (chức năng GPIO)
    uint32_t v = REG(IO_MUX_GPIO4_REG);
    v &= ~(0x7u << MCU_SEL_S);
    v |=  (1u   << MCU_SEL_S);
    REG(IO_MUX_GPIO4_REG) = v;

    // 2. GPIO matrix: chân 4 lấy tín hiệu từ thanh ghi GPIO_OUT
    REG(GPIO_FUNC0_OUT_SEL_CFG + 4 * LED_PIN) = SIG_GPIO_OUT_IDX;

    // 3. Bật output cho chân 4
    REG(GPIO_ENABLE_W1TS_REG) = (1u << LED_PIN);

    while (1) {
        REG(GPIO_OUT_W1TS_REG) = (1u << LED_PIN);   // LED sáng
        delay(8000000);
        REG(GPIO_OUT_W1TC_REG) = (1u << LED_PIN);   // LED tắt
        delay(8000000);
    }
}