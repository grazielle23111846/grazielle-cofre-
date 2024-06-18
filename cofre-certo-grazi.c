//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       _              //
//               _    _       _      _        _     _   _   _    _   _   _        _   _  _   _          //
//           |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|    //    
//         |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_   //
//                                                                                       /              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

// Área de inclusão das bibliotecas
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ioplaca.h"   // Controles das Entradas e Saídas digitais e do teclado
#include "lcdvia595.h" // Controles do Display LCD
#include "hcf_adc.h"   // Controles do ADC
#include "MP_hcf.h"   // Controles do ADC

// Área das macros
//------------------------------------------------------------------------------

// Área de declaração de variáveis e protótipos de funções
//-----------------------------------------------------------------------------

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0; //variáveis de controle de entradas e saídas
int controle =0; 
char mostra [40];   // variável para mostrar 
int num1=0;         // relativo ao numero um 
int num2 =0;        //relativo ao numero dois
int coluna =0;      // usado na calculadora
int resultado =0;   //usado na calculadora
char operador;      //usado na calculadora 
char tecla;         // variável para tecla 
int apaga = 0;      //usado na calculadora 
int motor = 0;      //usado em outro código 
uint32_t pot = 0;   // ver valor do potenciometro 
int i =0;           //variável para controle de abertura e fechamento 
int dig =0;         // variável para dígito
int senha = 2207;   //senha correta 



// Funções e ramos auxiliares
//-----------------------------------------------------------------------------------------------------------------------
void configura_io (){}

void mostra_lcd (void)  //atalho para mostrar valor do adc
{
     sprintf (&mostra [0], "%ld", pot);
    lcd595_write (2,0, &mostra [0]);
}

void abrir_cofre (void) // atalho para abrir cofre
{
    
    for ( int i =0; i < 1843; i = pot) //parametro de abertura 
    {
        rotacionar_DRV (1,10, saidas); // abre tampa
        hcf_adc_ler (&pot); // lê o potenciometro 
        //mostra_lcd(); //mostrar valor do lcd
    }
}

void fechar_cofre (void) //atalho para fechar cofre
{
  for (int i =pot; i > 280; i = pot) //parametro para fechamento
  {
    rotacionar_DRV (0, 10, saidas); //fecha tampa
    hcf_adc_ler (pot); // le o potenciometro 
    //mostra_lcd(); //mostrar valor do lcd
  }
}

// Programa Principal
//-----------------------------------------------------------------------------------------------------------------------

void app_main(void)
{
    /////////////////////////////////////////////////////////////////////////////////////   Programa principal
  
    // a seguir, apenas informações de console, aquelas notas verdes no início da execução
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    /////////////////////////////////////////////////////////////////////////////////////   Inicializações de periféricos (manter assim)
    
    // inicializar os IOs e teclado da placa
    ioinit();      
    entradas = io_le_escreve(saidas); // Limpa as saídas e lê o estado das entradas

    // inicializar o display LCD 
    lcd595_init();
    lcd595_write(1,1,"    Jornada 1   "); //escreve no display
    lcd595_write(2,1," Programa Basico"); //escreve no display 

    lcd595_init();
    lcd595_write(1,1,"  Grazielle  "); //escreve no display 
    lcd595_write(2,1," Cofre "); //escreve no display 

    // Inicializar o componente de leitura de entrada analógica
    esp_err_t init_result = hcf_adc_iniciar();
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar o componente ADC personalizado");
    }

    //delay inicial
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    lcd595_clear();

    /////////////////////////////////////////////////////////////////////////////   Periféricos inicializados


    //////////////////////////////////////////////////////////////////////////////   Início do ramo principal  

    //iniciar driver 
    DRV_init (6,7);
    lcd595_write(1,1, " Digite a Senha "); //escreve no display
    lcd595_write(2,1," [] [] [] [] ");  //escreve no display


    while (1)

    {
    
    entradas = io_le_escreve(saidas);
    hcf_adc_ler(&pot); // ler valor do adc

   /* sprintf (mostra, " %" PRIu32" ",pot);
    lcd595_write (1,1, mostra );*/

    tecla = le_teclado (); // teclado 

    if (tecla>='0' && tecla <='9')  
    {
    //if (controle ==0)
    {num1 = num1 * 10 + tecla -'0';}
     dig = dig + 1; //para funcionar os dígitos 

    }
    
    if (dig==1) // se tiver um digito, * na primeira posição//
    {lcd595_write(1,1," Digite a Senha ");//escreve no display 
     lcd595_write(2,1," [*] [ ] [ ] [ ] ");//escreve no display 
    }

    if (dig==2)  // se tiver dois digitos, ** nas duas primeiras posições//
    {lcd595_write(1,1," Digite a Senha ");//escreve no display 
     lcd595_write(2,1," [*] [*] [ ] [ ] ");//escreve no display 
    }

    if (dig==3)  // se tiver um digito, * nas tres primeiras posições//
    {lcd595_write(1,1," Digite a Senha ");//escreve no display 
     lcd595_write(2,1," [*] [*] [*] [ ] ");//escreve no display 
    }

    if (dig==4)  // se tiver quatro digitos, * em todas as posições//
    {lcd595_write(1,1," Digite a Senha ");//escreve no display 
     lcd595_write(2,1," [*] [*] [*] [*] ");//escreve no display 
    
      if (num1 == 2207)  //se tiver quatro digitos e a senha estiver correta, abrir cofre//
      {

        lcd595_write (1,1, "Senha Correta"  );//escreve no display 
        abrir_cofre (); //atalho para abrir cofre
        vTaskDelay (300/portTICK_PERIOD_MS); //delay para fechar
        vTaskDelay (1000/portTICK_PERIOD_MS); //delay para fechar
        lcd595_clear(); //limpa a ação anterior 
        lcd595_write (1,1, "10 segundos"); //escreve no display 
        vTaskDelay (1000/portTICK_PERIOD_MS); //delay para fechar
        lcd595_clear();//limpa a ação anterior  
        lcd595_write (1,1, "09 segundos"); //escreve no display 
        vTaskDelay (1000/portTICK_PERIOD_MS); //delay para fechar
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "08 segundos"); //escreve no display  
        vTaskDelay(1000 / portTICK_PERIOD_MS); //delay para fechar    
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "07 segundos"); //escreve no display 
        vTaskDelay(1000 / portTICK_PERIOD_MS); //delay para fechar   
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "06 segundos"); //escreve no display 
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar  
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "05 segundos"); //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar   
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "04 segundos");  //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar   
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "03 segundos"); //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar   
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "02 segundos"); //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar    
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, "01 segundos"); //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar   
        lcd595_clear ();//limpa a ação anterior 
        lcd595_write (1,1, " 0 segundos" ); //escreve no display
        vTaskDelay(1000 / portTICK_PERIOD_MS);//delay para fechar   
        lcd595_clear (); //limpa a ação anterior  
        fechar_cofre();// atalho para fechar cofre
      }
    
     else
     {
        lcd595_write(1,1, " Senha incorreta ");// senha incorreta 
        vTaskDelay(1000 / portTICK_PERIOD_MS); //delay  
        lcd595_write(1,1, " Digite a Senha "); //escreve no display
        lcd595_write(2,1," [] [] [] [] "); //escreve no display   
        controle = 0; //para reiniciar 
        num1 = 0; //para reiniciar 
        dig = 0;//para reiniciar 
     }
     
    }

    if (tecla == '/') // tecla de abrir //
     {
        rotacionar_DRV (1,10, saidas);
     }

    if (tecla == 'x') //tecla de fechar//
     {rotacionar_DRV (0,10, saidas);
     }

        //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - - -  -  -  -  -  -  -  -  -  -  Escreva seu só até aqui!!! //
        //________________________________________________________________________________________________________________________________________________________//
        vTaskDelay(10 / portTICK_PERIOD_MS);    // delay mínimo obrigatório, se retirar, pode causar reset do ESP
    
    }
    // caso erro no programa, desliga o módulo ADC
    hcf_adc_limpar();

    /////////////////////////////////////////////////////////////////////////////////////   Fim do ramo principal
    
}








    


                       
    

