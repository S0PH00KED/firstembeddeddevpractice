#include <stdint.h>

#define PERIPH_BASE        0x40000000UL
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x20000UL)  // 0x40020000
#define RCC_BASE           (AHBPERIPH_BASE + 0x1000UL) // 0x40021000

#define IOPORT_BASE        0x50000000UL
#define GPIOA_BASE         (IOPORT_BASE + 0x0000UL)

#define RCC_IOPENR         (*(volatile uint32_t *)(RCC_BASE + 0x2C))
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR          (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA ((volatile uint32_t*) GPIOA_BASE)

#define RCC_IOPENR_GPIOA   (1 << 0)

#define SDEL 100000
#define LDEL 300000

void delay(volatile uint32_t t)
{
    while (t--) __asm__("nop");
}

void led_toggle(uint8_t pin) {
	//Previous code in main:
	//GPIOA_ODR ^= (1 << 5);
	GPIOA_ODR ^= (1u << pin); // Flip (invert) the output state of the selected pin using XOR.
}

void led_on(uint8_t pin) {
	GPIOA_ODR |= (1u << pin);
}

void led_off(uint8_t pin) {
	GPIOA_ODR &= ~(1u << pin);
}

void dot(uint8_t pin) {
	led_on(pin);
	delay(SDEL);
	led_off(pin);
	delay(SDEL);
}

void dash(uint8_t pin) {
	led_on(pin);
	delay(LDEL);
	led_off(pin);
	delay(LDEL);
}

void gpio_init_led(uint8_t pin) {
	//Previous code in main:
    //GPIOA_MODER &= ~(3 << (5 * 2)); // clear bits 10-11
    //GPIOA_MODER |=  (1 << (5 * 2)); // set bit for output mode
    GPIOA_MODER &= ~(3u << (pin * 2)); // clear bits whatever pin we pass.
    GPIOA_MODER |=  (1u << (pin * 2)); // set bit for output mode
}

void blink_patternSOS(uint8_t pin) {

	for (int x = 0; x < 3; x++) {
		dot(pin);
	}
	for (int x = 0; x < 3; x++) {
		dash(pin);
	}
	for (int x = 0; x < 3; x++) {
		dot(pin);
	}
	delay(1000000);
}

int main(void)
{

    // enable GPIOA clock
    RCC_IOPENR |= RCC_IOPENR_GPIOA;
    /*
	GPIOA[0] = 0x00000000; //MODER
	GPIOA[2] = 0x00000000; // OSPEEDR
	GPIOA[3] = 0x00000000; //PUPDR
	*/
    // set PA5 to output mode
    gpio_init_led(5);

    while (1)
    {
        blink_patternSOS(5);
    }
}
