# BITDOG_AD

## Tarefa 2 - Aula Síncrona 10/02

### Integrante
- Marcel Mascarenhas Andrade

### Pré-requisitos
Certifique-se de que você tem o ambiente configurado conforme abaixo:
- **Pico SDK** instalado e configurado.
- **VSCode** com todas as extensões necessárias configuradas.
- **CMake** e **Make** instalados.

### Instruções de Configuração
1. Clone o repositório e abra a pasta do projeto no VSCode.
   ```sh
   git clone https://github.com/mmandradeuesc/bitdog_ad
   cd bitdog_ad
   code .

### Manual do Programa
Objetivos
• Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
• Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
• Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
• Aplicar o protocolo de comunicação I2C na integração com o display.

Descrição do Projeto:
O joystick fornecerá valores analógicos correspondentes aos eixos X e Y, que serão utilizados para:
Controlar a intensidade luminosa dos LEDs RGB, onde:
• O LED Azul terá seu brilho ajustado conforme o valor do eixo Y. Quando o joystick estiver solto
(posição central - valor 2048), o LED permanecerá apagado. À medida que o joystick for movido para
cima (valores menores) ou para baixo (valores maiores), o LED aumentará seu brilho gradualmente,
atingindo a intensidade máxima nos extremos (0 e 4095).
• O LED Vermelho seguirá o mesmo princípio, mas de acordo com o eixo X. Quando o joystick estiver
solto (posição central - valor 2048), o LED estará apagado. Movendo o joystick para a esquerda
(valores menores) ou para a direita (valores maiores), o LED aumentará de brilho, sendo mais intenso
nos extremos (0 e 4095).
• Os LEDs serão controlados via PWM para permitir variação suave da intensidade luminosa.
Exibir no display SSD1306 um quadrado de 8x8 pixels, inicialmente centralizado, que se moverá
proporcionalmente aos valores capturados pelo joystick.
Adicionalmente, o botão do joystick terá as seguintes funcionalidades:
• Alternar o estado do LED Verde a cada acionamento.
• Modificar a borda do display para indicar quando foi pressionado, alternando entre diferentes estilos
de borda a cada novo acionamento.
Finalmente, o botão A terá a seguinte funcionalidade:
• Ativar ou desativar os LED PWM a cada acionamento.
Neste projeto, deverá ser utilizados os seguintes componentes já interconectados à placa BitDogLab:
• LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
• Botão do Joystick conectado à GPIO 22.
• Joystick conectado aos GPIOs 26 e 27.
• Botão A conectado à GPIO 5.
• Display SSD1306 conectado via I2C (GPIO 14 e GPIO15)

### Documentação do Sistema de Controle com Joystick

1. Visão Geral
Este programa implementa um sistema interativo utilizando o microcontrolador RP2040, onde um joystick controla tanto a intensidade de LEDs RGB quanto a posição de um quadrado em um display OLED. O sistema integra entradas analógicas, saídas PWM e comunicação I2C.
2. Componentes de Hardware

Microcontrolador: RP2040 (BitDogLab)
Display: OLED SSD1306 (I2C)
LED RGB (3 cores)
Joystick analógico com botão
Botão adicional (Botão A)

2.1. Mapeamento de Pinos

LED Vermelho: GPIO 13 (PWM)
LED Verde: GPIO 11 (GPIO padrão)
LED Azul: GPIO 12 (PWM)
Joystick X: GPIO 26 (ADC)
Joystick Y: GPIO 27 (ADC)
Botão do Joystick: GPIO 22
Botão A: GPIO 5
Display I2C: GPIO 14 (SDA) e GPIO 15 (SCL)

3. Funcionalidades Principais
3.1. Controle de LEDs
LED Vermelho (PWM)

Controlado pelo eixo X do joystick
Intensidade mínima (apagado) na posição central (≈2048)
Intensidade aumenta progressivamente ao mover para esquerda ou direita
Intensidade máxima nos extremos (0 e 4095)

LED Azul (PWM)

Controlado pelo eixo Y do joystick
Intensidade mínima (apagado) na posição central (≈2048)
Intensidade aumenta progressivamente ao mover para cima ou baixo
Intensidade máxima nos extremos (0 e 4095)

LED Verde (Digital)

Controlado pelo botão do joystick
Alterna entre ligado e desligado a cada pressionar do botão

3.2. Display OLED
Quadrado Móvel

Tamanho: 8x8 pixels
Posição inicial: Centro do display
Movimento: Controlado pelos eixos X e Y do joystick
Movimento proporcional aos valores analógicos do joystick

Borda do Display

Três estilos diferentes de borda:

Sólida: Linha contínua
Tracejada: Traços espaçados regularmente
Pontilhada: Pontos espaçados regularmente


Alterada pelo botão do joystick
Espessura: 2 pixels para melhor visibilidade

3.3. Controle por Botões
Botão do Joystick

Alterna o estado do LED Verde
Modifica o estilo da borda do display
Implementa debounce de 200ms

Botão A

Ativa/Desativa os LEDs controlados por PWM (Vermelho e Azul)
Quando desativado, ambos os LEDs PWM são completamente apagados

4. Implementação Técnica
4.1. Conversão ADC

Resolução: 12 bits (0-4095)
Zona morta central: 1948-2148
Mapeamento não-linear para movimento suave

4.2. Controle PWM

Resolução: 8 bits (0-255)
Frequência: Determinada pelo clock do sistema
Mapeamento proporcional dos valores ADC para PWM

4.3. Otimizações

Atualização do display apenas quando necessário
Debounce em botões para evitar falsos acionamentos
Verificação de limites para evitar desenho fora da área do display

5. Limitações e Considerações

O display é atualizado apenas quando há mudança na posição do quadrado ou no estilo da borda
O sistema implementa verificações de limites para garantir que o quadrado não ultrapasse as bordas do display
A intensidade dos LEDs PWM é controlada de forma não-linear para melhor percepção visual

6. Dependências

pico/stdlib.h: Funções básicas do RP2040
hardware/adc.h: Controle do ADC
hardware/i2c.h: Comunicação I2C
hardware/pwm.h: Controle PWM
ssd1306.h: Biblioteca para controle do display OLED
 
- Link do vídeo demonstração do programa:
https://drive.google.com/file/d/1GysPOs8tkBxzOAAImCkJyorEIcscwWEf/view?usp=sharing
