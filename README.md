# ft_traceroute

Proyecto `ft_traceroute` de 42. Guía paso a paso para abordarlo de forma correcta, sin dar código, pero con todos los detalles necesarios para una implementación robusta y cercana a la realidad.

---

## **1️⃣ Objetivo del proyecto**

`ft_traceroute` permite **visualizar la ruta que siguen los paquetes ICMP desde tu máquina hasta un host destino**:

* Envía paquetes ICMP Echo Request con **TTL creciente**.
* Cada router intermedio que recibe un paquete con TTL=0 responde con **ICMP Time Exceeded**.
* Cuando el paquete llega al destino, este responde con **ICMP Echo Reply**.
* Con esto, podemos mostrar **cada salto** y su **RTT (latencia ida/vuelta)**.

---

## **2️⃣ Preparación del entorno**

1. **Docker opcional, pero recomendado**:

   * Contenedor Linux limpio para pruebas.
   * Evita conflictos con la red de tu máquina.
   * Necesitarás **permisos root** para raw sockets ICMP.

2. **ft_ping funcional como referencia**:

   * Sirve como base conceptual para enviar y recibir ICMP.
   * Útil para validar cálculo de RTT y parsing de ICMP.

---

## **3️⃣ Manejo de sockets y TTL**

* Crear **socket RAW ICMP**: `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`.
* Configurar **TTL inicial** con `setsockopt(IPPROTO_IP, IP_TTL, ...)`.
* Configurar **timeout de recepción** con `select` o `SO_RCVTIMEO`.
* No usar `fcntl`, `poll` ni `ppoll` (prohibido por specs).
* Se puede usar `sendto` para enviar paquetes y `recvfrom` para recibir ICMP.

---

## **4️⃣ Algoritmo principal**

1. **Inicialización**:

   * Parsear argumentos: `host` y `--help`.
   * Resolver IP del destino con `getaddrinfo` o `inet_pton`.
   * Preparar sockets y variables.

2. **Bucle de TTL**:

```
TTL = 1
while TTL <= max_hops:
    enviar paquete ICMP con TTL actual
    esperar respuesta ICMP con timeout
    si ICMP recibido:
        guardar timestamp
        calcular RTT
        si Time Exceeded → salto intermedio
        si Echo Reply → destino alcanzado, terminar
    si timeout:
        mostrar '*'
    TTL++
```

* Opcional: enviar **3 intentos por TTL** y mostrar cada RTT o `*`.

---

## **5️⃣ Parsing de ICMP**

* Recibir ICMP:

  * **Time Exceeded** → salto intermedio.
  * **Echo Reply** → destino alcanzado.
* Verificar **ID del paquete** (`getpid() & 0xFFFF`) para asegurar que corresponde a nuestro envío.
* Extraer **IP del salto** desde `sockaddr_in` o cabecera IP dentro del payload ICMP.

---

## **6️⃣ RTT (Round-Trip Time)**

* **Definición**: tiempo total ida y vuelta de un paquete ICMP.
* **Medición**:

  1. Guardar tiempo antes de enviar paquete.
  2. Guardar tiempo al recibir respuesta ICMP.
  3. RTT = t2 - t1 (en ms, tolerancia ±30ms).
* Mostrar RTT en cada línea junto a IP del salto.

---

## **7️⃣ Formato de salida**

* Cada línea corresponde a un TTL (salto):

```
1  192.168.0.1  1.234 ms  1.345 ms  1.456 ms
2  10.0.0.1     2.123 ms  2.234 ms  2.345 ms
3  * * *
```

* `*` indica timeout.
* Mostrar hostname opcional solo en bonus.

---

## **8️⃣ Consideraciones importantes**

* Privilegios root para sockets RAW ICMP.
* Endianness: usar `htons` y `htonl` cuando sea necesario.
* Control de señales: atrapar `SIGINT` para cerrar sockets y limpiar.
* Funciones permitidas: `gettimeofday`, `select`, `setsockopt`, `sendto`, `recvfrom`, `inet_pton`, `getaddrinfo`, `getnameinfo`, `printf`, `exit`, `malloc/free`.
* IPv4 obligatorio; IPv6 solo para bonus.
* Bonus solo evaluados si la parte obligatoria funciona perfectamente.

---

## **9️⃣ Plan de desarrollo sugerido**

1. Parsear argumentos y mostrar `--help`.
2. Resolver destino a IP.
3. Crear socket RAW ICMP.
4. Implementar envío de paquete ICMP con TTL=1.
5. Recibir ICMP y medir RTT.
6. Mostrar salto y RTT.
7. Incrementar TTL hasta destino o TTL máximo.
8. Añadir múltiples intentos por TTL (opcional).
9. Manejar timeouts con `*`.
10. Test exhaustivo en hosts locales y públicos.
11. Bonus: resolución DNS de saltos, flags adicionales, estadísticas completas.

---

## **10️⃣ Pruebas recomendadas**

* `127.0.0.1` → 1 salto.
* LAN local → 2-3 saltos.
* Host público → varios saltos.
* Host inalcanzable → `*`.
* TTL demasiado bajo → paquetes Time Exceeded.
* Ctrl+C → limpieza de sockets y salida limpia.

---

```
         ┌─────────────────────────────┐
         │          Inicio             │
         └──────────────┬──────────────┘
                        │
         ┌──────────────▼──────────────┐
         │ Parsear argumentos (--help) │
         │ Resolver host → IP          │
         └──────────────┬──────────────┘
                        │
         ┌──────────────▼──────────────┐
         │   Crear socket RAW ICMP     │
         │   Configurar timeout / TTL  │
         └──────────────┬──────────────┘
                        │
           TTL = 1      │
                        ▼
         ┌───────────────────────────┐
         │ Mientras TTL <= max_hops  │
         └──────────────┬────────────┘
                        │
         ┌──────────────▼───────────────────────┐
         │ Por intento = 1 a 3 (3 paquetes TTL) │
         └──────────────┬───────────────────────┘
                        │
                ┌───────▼─────────┐
                │ Enviar ICMP     │
                │ Echo Request    │
                └───────┬─────────┘
                        │
                ┌───────▼─────────┐
                │ Guardar t1      │
                └───────┬─────────┘
                        │
                ┌───────▼────────────┐
                │ Esperar ICMP       │
                │ (select / timeout) │
                └───────┬────────────┘
                        │
       ┌────────────────┴───────────────┐
       │                                │
 ┌─────▼─────┐                     ┌────▼────┐
 │ ICMP      │                     │ Timeout │
 │ recibido? │                     │         │
 └─────┬─────┘                     └────┬────┘
       │ Sí                             │
       ▼                                ▼
┌──────────────┐           Mostrar '*' (sin respuesta)
│ Guardar t2   │
│ Calcular RTT │
└──────┬───────┘
       │
┌──────▼─────────┐
│ Analizar ICMP  │
│ Time Exceeded? │─────────────┐
│ Echo Reply?    │             │
└──────┬─────────┘             │
       │ Sí                    │
       ▼                       ▼
Mostrar IP + RTT     Destino alcanzado → Terminar
       │
TTL++
       │
Repetir ciclo hasta max_hops o destino
```

### **Notas prácticas:**

* **TTL**: empieza en 1, aumenta hasta el máximo definido o hasta que el destino responda.
* **3 intentos por TTL**: opcional pero recomendado, mostrar RTTs separados.
* **Timeouts**: muestra `*` si no llega respuesta.
* **RTT**: calcula con `gettimeofday` antes y después de recibir ICMP.
* **ICMP recibido**: diferencia entre **Time Exceeded** (router intermedio) y **Echo Reply** (destino alcanzado).

---

# ✅ **Bonus para ft_traceroute - Versión Corregida**

## **1. DNS Management (FÁCIL)**

**¿Qué es?**
Hacer reverse DNS para cada salto usando `getnameinfo`, y mostrar "hostname (IP)" en vez de solo la IP.

**Por qué es fácil:**
* Ya tienes la IP de cada salto.
* `getnameinfo()` ya está autorizado.
* No rompe nada si falla → simplemente muestras la IP.

**Dificultad:** ⭐

---

## **2. Flag `-m <max_ttl>` → Cambiar TTL máximo** ✅ **IMPLEMENTADO**

**¿Qué es?**
Permitir que el usuario elija el máximo número de saltos, igual que `traceroute -m 20`.

**Por qué es fácil:**
* El bucle TTL ya existe.
* Solo cambias el límite (por defecto es 30).
* No requiere modificar sockets ni lógica compleja.

**Dificultad:** ⭐

---

## **3. Flag `-q <nprobes>` → Número de probes por salto** ✅ **IMPLEMENTADO**

**¿Qué es?**
Permitir elegir cuántos "intentos" harás por TTL (por defecto 3).

**Por qué es fácil:**
* Ya tienes un bucle interno que envía 3 paquetes.
* Solo haces ese valor configurable.
* Límite real: máximo 10 probes.

**Dificultad:** ⭐⭐

---

## **4. Flag `-i <device>` → Interfaz de red** ✅ **IMPLEMENTADO**

**¿Qué es?**
Especificar la interfaz de red por la que se enviarán los paquetes.

**Por qué es fácil:**
* Se implementa con `setsockopt` y `SO_BINDTODEVICE`.
* Validación simple del nombre de la interfaz.

**Dificultad:** ⭐⭐

---

## **5. Flag `-t <tos>` → Type of Service (IPv4)** ✅ **IMPLEMENTADO**

**¿Qué es?**
Establecer el campo TOS (Type of Service) en el encabezado IP.

**Por qué es fácil:**
* Se implementa con `setsockopt` y `IP_TOS`.
* Validación de rango (0-255).

**Dificultad:** ⭐⭐

---

## **6. Flag `-w <timeout>` → Timeout para recvfrom**

**¿Qué es?**
Permitir cambiar el timeout de espera de respuesta por salto.

**Por qué es fácil:**
* Ya estás usando `SO_RCVTIMEO` en el socket.
* Cambias el valor fijo → variable.

**Dificultad:** ⭐⭐

---

## **7. Flag `-z <interval>` → Intervalo entre envíos**

**¿Qué es?**
Esperar X milisegundos entre probes.

**Por qué es fácil:**
* Solo introduces un `usleep()` o `nanosleep()`.
* No afecta a la lógica del socket.

**Dificultad:** ⭐⭐

---

# 🎯 **Lista actualizada de bonus**

| Bonus              | Dificultad | Estado       | Descripción                        |
|--------------------|------------|--------------|------------------------------------|
| **DNS Management** | ⭐         | Pendiente    | Mostrar hostname con getnameinfo   |
| **-m <max_ttl>**   | ⭐         | Implementado | Cambiar TTL máximo                 |
| **-q <nprobes>**   | ⭐⭐       | Implementado | Cambiar número de probes por salto |
| **-i <device>**    | ⭐⭐       | Implementado | Interfaz de red                    |
| **-t <tos>**       | ⭐⭐       | Implementado | Type of Service (IPv4)             |
| **-w <timeout>**   | ⭐⭐       | Pendiente    | Timeout configurable               |
| **-z <interval>**  | ⭐⭐       | Pendiente    | Intervalo entre probes             |

---

# 🧠 Bonus difíciles

| Flag       | Dificultad | Motivo                                         |
|------------|------------|------------------------------------------------|
| `-p`, `-s` | 🔥         | Implica modificar puertos y sockets            |
| `-l`       | 🔥         | Raw sockets diferentes, parsing complejo       |
| `-N`       | 🔥🔥       | MPLS, ICMP Extensions, parsing complejo        |
| `-T`, `-U` | 🔥🔥🔥     | Cambiar de ICMP a TCP/UDP requiere otro socket |

---

## Análisis Detallado de `ft_traceroute`

El programa `ft_traceroute` funciona esencialmente enviando paquetes UDP con un Time-To-Live (TTL) creciente y escuchando las respuestas ICMP generadas por los *routers* intermedios (Time Exceeded) o por el destino final (Destination Unreachable).

### 1. `main` (El Flujo Principal)

La función `main` coordina toda la ejecución, desde la configuración inicial hasta el manejo de errores y la limpieza.

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **Inicialización y `malloc`** | Reserva memoria para la estructura `conf`, que centraliza todas las configuraciones y estados (destino, TTL máximo, número de sondas, sockets, etc.). |
| **`init_signal()`** | (Función no mostrada) Establece un *handler* para la señal **SIGINT** (Ctrl+C). Este *handler* simplemente pone la variable `g_sigint_received` a `1`. Esto permite una salida limpia del bucle principal de escaneo en lugar de terminar el programa inmediatamente. |
| **`init_struct(conf)`** | Inicializa todos los campos de `conf` a sus valores por defecto (ej. `max_ttl = 30`, `nprobes = 3`, `payload_size = 32`). **Aquí se calcula `conf->packet_size` (generalmente $60 \text{ bytes}$) a partir de las cabeceras fijas y el *payload* por defecto.** |
| **`ft_parser(conf, argv, argc)`** | Procesa los argumentos de la línea de comandos (`-m`, `-q`, `-i`, `-t`). Si encuentra errores (ej. argumento faltante o valor inválido), establece `exit = 1`. **Bonus implementado:** Manejo de opciones y sus argumentos. |
| **`conf->show_help` / `conf->show_version`** | Si el *parser* valida el comando y se solicitó ayuda o versión, se muestran y el programa finaliza sin necesidad de sockets ni DNS. |
| **`dns_resolution(conf)`** | (Función no mostrada) Resuelve el nombre de *host* proporcionado (ej. `google.es`) en una dirección IP numérica (`struct in_addr`), almacenando el resultado en `conf->ip_address`. Esto es esencial porque los sockets solo funcionan con direcciones numéricas. |
| **`socket_creation(conf)`** | Crea el socket de envío (UDP), explicado en detalle más adelante. |
| **Impresión del Encabezado** | Una vez que se tiene el *host* y la IP resuelta, se imprime la línea inicial: `traceroute to <hostname> (<IP>), <max_ttl> hops max, <packet_size> byte packets`. |
| **`send_socket(conf)`** | La lógica central del escaneo, explicada en detalle más adelante. |
| **`cleanup(conf)`** | Cierra el socket abierto (`conf->sockfd`) y libera la memoria reservada para la estructura `conf`, asegurando que no haya fugas de recursos. |

### 2. `socket_creation` (Configurando el Socket de Envío)

Esta función crea y configura el socket que se usará para enviar los paquetes UDP.

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **`socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)`** | Crea el socket principal de envío. El uso de: $\text{AF\_INET}$ (IPv4), $\text{SOCK\_DGRAM}$ (Datagrama, para UDP), e $\text{IPPROTO\_UDP}$ define el tipo de tráfico que se generará: paquetes UDP. |
| **`setsockopt(..., SO_RCVTIMEO, ...)`** | Establece un tiempo de espera de $4$ segundos para la recepción en este socket. Aunque la recepción ICMP se hace en un socket *RAW* separado, esta configuración es una buena práctica para el socket de envío UDP. |
| **`#ifdef SO_BINDTODEVICE`... (`-i` Bonus)** | **Bonus de Interfaz (`-i`)** Si la macro `SO_BINDTODEVICE` está definida (común en Linux) y el usuario especificó una interfaz (`conf->interface != NULL`), esta opción obliga a que los paquetes salientes usen **exclusivamente** la tarjeta de red especificada (ej. `eth0`, `wlan0`). |

### 3. `send_socket` (El Corazón del Escaneo)

Esta es la función más compleja y contiene el doble bucle principal (TTL y Sondas).

#### 3.1. Preparación de Sockets y Bucle TTL

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **`recv_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`** | Crea el socket de recepción. Debe ser un socket **RAW** porque el programa necesita leer los paquetes ICMP (errores de `Time Exceeded` o `Destination Unreachable`), que se encuentran por encima de la capa IP y no son gestionados por los sockets de alto nivel (como TCP o UDP). **Esto requiere permisos de administrador (`sudo`).** |
| **`setsockopt(recv_sock, ..., SO_RCVTIMEO, ...)`** | Establece un tiempo de espera de $1$ segundo para la recepción en este socket RAW. Si en $1$ segundo no llega una respuesta ICMP (del *router* o del destino), `recvfrom` fallará (timeout), y el programa imprimirá un `*`. |
| **Bucle `for (ttl = 1; ttl <= conf->max_ttl ...)`** | El bucle externo itera a través del **Time-To-Live**, incrementándolo en $1$ en cada paso. $\text{TTL} = 1$ va al primer *router*, $\text{TTL} = 2$ al segundo, y así sucesivamente, hasta alcanzar el destino o `conf->max_ttl` (por defecto $30$). |
| **`setsockopt(conf->sockfd, IPPROTO_IP, IP_TTL, ...)`** | **CRÍTICO:** Antes de cada salto, se establece el valor actual de $\text{TTL}$ en el socket UDP. Este es el mecanismo de **`traceroute`**: forzar la expiración del paquete en el *router* deseado para que este responda con un ICMP $\text{Time Exceeded}$. |
| **`printf("%2d  ", ttl)`** | Imprime el número de salto actual. |

#### 3.2. Bucle de Sondas (Probes) y Envío/Recepción

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **`in_addr_t last_ip = 0;`** | Inicialización del *bonus* de multipath. Esta variable rastrea la dirección IP del último *router* que respondió en este salto (`ttl` actual). |
| **Bucle `for (int probe = 0; probe < conf->nprobes ...)`** | Bucle interno que itera sobre el número de sondas por salto (por defecto $3$, configurable con `-q`). |
| **`dest.sin_port = htons(port + ttl);`** | Se usa un puerto de destino que aumenta con el $\text{TTL}$ (ej. $33434 + \text{TTL}$). Este es el puerto inusual al que el programa intenta llegar. Cuando el paquete alcanza el destino final, el sistema operativo ve que ningún servicio usa ese puerto, y por convención, responde con un ICMP $\text{Destination Unreachable}$, que es la señal de terminación de `traceroute`. |
| **`gettimeofday(&start, NULL);`** | Marca el tiempo exacto antes de enviar la sonda. |
| **`sendto(..., payload_data, conf->payload_size, ...)`** | Envía el paquete UDP al destino, pero con el $\text{TTL}$ configurado para expirar en el *router* deseado. `conf->payload_size` asegura que se envíe la cantidad correcta de datos (por defecto $32 \text{ bytes}$). |
| **`recvfrom(recv_sock, buffer, ...)`** | Bloquea la ejecución, esperando la respuesta ICMP en el socket RAW. Si no llega nada en $1$ segundo (por el `timeout` configurado), falla. |
| **Cálculo RTT** | $\text{RTT}$ (Round Trip Time) es el tiempo transcurrido (en milisegundos) entre `start` y `end`. |

#### 3.3. Manejo de Multipath (Bonus)

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **`in_addr_t current_ip = recv_addr.sin_addr.s_addr;`** | Captura la IP de origen del paquete ICMP de respuesta (es decir, la dirección del *router*). |
| **`if (current_ip != last_ip)`** | **Lógica de Impresión Multipath:** Si la IP de respuesta es diferente a la última IP impresa en este salto, significa que el paquete tomó un camino diferente (balanceo de carga o multipath). |
| **`getnameinfo(...)`** | Intenta resolver la dirección IP del *router* en un nombre de *host* (DNS inverso). Si tiene éxito, se imprime el nombre de *host* y la IP. Si falla, solo se imprime la IP. |
| **`last_ip = current_ip;`** | **Actualización:** Si se imprime una nueva IP, se actualiza `last_ip`. Si la IP es la misma, solo se imprime el $\text{RTT}$ sin la IP. Esto replica el comportamiento del `traceroute` estándar. |

#### 3.4. Detección de Destino Alcanzado

| Segmento de Código | Propósito y Detalle Técnico |
| :--- | :--- |
| **`struct iphdr *ip_hdr = ...`** | El paquete ICMP recibido contiene el encabezado IP original del paquete que causó el error. Accedemos a la cabecera IP para saber su longitud.  |
| **`struct icmphdr *icmp_hdr = ...`** | Accedemos a la cabecera ICMP. |
| **`if (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH)`** | Esta es la condición de terminación. Un `ICMP_DEST_UNREACH` con código $\text{PORT\_UNREACH}$ significa: "Llegué a la máquina de destino, pero el puerto de destino $33434+TTL$ no está abierto". |
| **`ttl = conf->max_ttl; break;`** | Si se alcanza el destino, se establece `ttl` al máximo para salir del bucle exterior después de esta sonda, finalizando la ejecución. |


```
docker run -it --rm \
  --cap-add=NET_RAW \
  -v "$(pwd):/workspace" \
  -w /workspace \
  ubuntu:24.04 \
  bash -c "apt update && apt install -y build-essential iproute2 traceroute nmap && bash"
```