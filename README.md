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