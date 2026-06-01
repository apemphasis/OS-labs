# output

`int _cputs(const char *str)` - для переноса строки `\r\n`, выводит в консоль без буферизации

```C
int _cprintf(
	const char * format [, argument_list]
);
```
Форматированный вывод


# input

`int _getch( void );`  -  не буферизированный ввод символа без эхо, возвращает код символа


# format

```C
int wsprintf(
	output_buffer,
	const char * strf, 
	any arg 
);

...

char lpszComLine[80] ;
wsprintf(lpszComLine, "C:\\ConsoleProcess.exe %d", (int)hThread);
```