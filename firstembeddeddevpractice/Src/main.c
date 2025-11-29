#include <stdint.h>

#define PERIPH_BASE        0x40000000UL
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x20000UL)  // 0x40020000
#define RCC_BASE           (AHBPERIPH_BASE + 0x1000UL) // 0x40021000

#define IOPORT_BASE        0x50000000UL
#define GPIOA_BASE         (IOPORT_BASE + 0x0000UL)

#define RCC_IOPENR         (*(volatile uint32_t *)(RCC_BASE + 0x2C))
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR		   (*(volatile uint32_t *)(GPIOA_BASE + 0x10))
#define GPIOA_ODR          (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

//SysTick register defines
#define SYST_CSR   (*(volatile uint32_t *)0xE000E010UL)
#define SYST_RVR   (*(volatile uint32_t *)0xE000E014UL)
#define SYST_CVR   (*(volatile uint32_t *)0xE000E018UL)

#define SYST_CFLAG (1u << 16)
#define SYST_CLKSOURCE (1u << 2)
#define SYST_ENABLE (1u << 0)
#define SYST_TICKINT (1u << 1)

#define GPIOA ((volatile uint32_t*) GPIOA_BASE)
#define RCC_IOPENR_GPIOA   (1 << 0)

uint32_t systick_ms = 0;

void interrupt_systick(void) {

	SYST_RVR = 2100 - 1;
	uint32_t mask = SYST_TICKINT | SYST_ENABLE | SYST_CLKSOURCE;
	SYST_CSR = (mask);
}

//Cortex-M interrupt handlers must match the symbol names or the CPU will never call them.
void SysTick_Handler(void) {
	systick_ms++;
}

void led_toggle(uint8_t pin) {
	GPIOA_ODR ^= (1u << pin); // Flip (invert) the output state of the selected pin using XOR.
}

void led_on(uint8_t pin) {
	GPIOA_ODR |= (1u << pin);
}

void led_off(uint8_t pin) {
	GPIOA_ODR &= ~(1u << pin);
}

void gpio_init_led(uint8_t pin) {
    GPIOA_MODER &= ~(3u << (pin * 2)); // clear bits whatever pin we pass.
    GPIOA_MODER |=  (1u << (pin * 2)); // set bit for output mode
}

void gpio_init_flipz(uint8_t pin) {
    // clear the 2-bit MODER field for pin -> sets it to 00 (input) for use by Flipper Zero GPIO output program.
    GPIOA_MODER &= ~(3u << (pin * 2));
}

int main(void)
{

    // enable GPIOA clock
    RCC_IOPENR |= RCC_IOPENR_GPIOA;

	uint32_t last_t = 0;
	interrupt_systick();
    // Set PA0 to input mode
    gpio_init_flipz(0);
    // set PA5 to output mode
    gpio_init_led(5);

    while (1) {
        uint32_t now   = systick_ms;
        uint32_t input = GPIOA_IDR & (1u << 0);

        if (input) {
            // button pressed > run blink scheduler
            if (now - last_t >= 250) {
                led_toggle(5);
                last_t = now;
            }
        } else {
            // button not pressed > force LED off
            led_off(5);
        }
    }
}
