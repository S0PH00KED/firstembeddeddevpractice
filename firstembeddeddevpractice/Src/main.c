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
//Defines SYST_CFLAG as enabled at bit 16.
#define SYST_CFLAG (1u << 16)
//Defines SYST_CLKSOURCE as enabled at bit 2.
#define SYST_CLKSOURCE (1u << 2)
//defines SysTick at bit 0, as enabled.
#define SYST_ENABLE (1u << 0)

#define GPIOA ((volatile uint32_t*) GPIOA_BASE)
#define RCC_IOPENR_GPIOA   (1 << 0)

#define SDEL 100000
#define LDEL 300000

void systick_init(volatile uint32_t tick) {
	SYST_RVR = tick - 1;
	SYST_CVR = 0;
	//Here I got confused, I originally had added SYST_ENABLE and SYST_CLKSOURCE together.
	//Instead we OR them which combines the two bits into a single configuration value.
	//Bit position 0 holds value 1, bit position 2 holds value 4
	//Remember: bit positions represent a power of 2 because of positional notation.

	uint32_t mask = SYST_ENABLE | SYST_CLKSOURCE;
	//And OR them together. SYST_CSR is a 32 bit register. So our mask is something like
	//0000 0000 0000 0000 0101
	SYST_CSR = (mask);
}

//Implementing our systick.
void systick_delay(uint32_t tick) {
	systick_init(tick);
	while ((SYST_CSR & SYST_CFLAG) == 0) {
		//wait
	}
	SYST_CSR = 0;
}

/* First iteration of a delay depended on clock speed.
void delay(volatile uint32_t t)
{
    while (t--) __asm__("nop");
}
*/

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
	systick_delay(SDEL);
	led_off(pin);
	systick_delay(SDEL);
}

void dash(uint8_t pin) {
	led_on(pin);
	systick_delay(LDEL);
	led_off(pin);
	systick_delay(LDEL);
}

void gpio_init_led(uint8_t pin) {
    GPIOA_MODER &= ~(3u << (pin * 2)); // clear bits whatever pin we pass.
    GPIOA_MODER |=  (1u << (pin * 2)); // set bit for output mode
}

void gpio_init_flipz(uint8_t pin) {
    // clear the 2-bit MODER field for pin -> sets it to 00 (input) for use by Flipper Zero GPIO output program.
    GPIOA_MODER &= ~(3u << (pin * 2));
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

    // Set PA0 to input mode
    gpio_init_flipz(0);
    // set PA5 to output mode
    gpio_init_led(5);

    /*while (1)
    {
        blink_patternSOS(5);
    }
    */
    while (1) {
    	if (GPIOA_IDR & (1u << 0)) {
    		led_on(5);
    	}
    	else {
    		led_off(5);
    	}
    }
}
