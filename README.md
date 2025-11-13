# ft_traceroute

Proyecto `ft_traceroute` de 42. Guia paso a paso para abordarlo de forma correcta, sin el código, pero con todos los detalles necesarios para que la implementación sea robusta y cercana a la de la realidad. Estructurado como un **plan completo de trabajo**, con objetivos, conceptos, flujo de ejecución y consejos prácticos.

---

## **1️⃣ Comprender qué hace `traceroute`**

Antes de empezar a codificar, debes tener claro qué es lo que hace:

* `traceroute` permite **mapear la ruta que siguen los paquetes hasta un host**.
* Funciona enviando **paquetes UDP (por defecto) o ICMP** con **TTL (Time To Live) crecientes**.
* Cada router que recibe un paquete con TTL = 1 lo descarta y envía un **ICMP "Time Exceeded"**.
* Cuando el paquete llega al destino final, este responde con **ICMP Port Unreachable** (si es UDP) o **ICMP Echo Reply** (si es ICMP Echo Request).
* Así, `traceroute` puede construir la ruta mostrando cada salto y su tiempo de respuesta.

**Resumen del flujo:**

```
Inicio -> enviar paquete con TTL=1 -> esperar ICMP
      -> recibir ICMP Time Exceeded -> imprimir IP + RTT
      -> TTL++
      -> repetir hasta llegar al destino
```

---

## **2️⃣ Decidir el tipo de paquetes**

* **UDP** (tradicional de `traceroute`):

  * Envías paquetes UDP a puertos altos (por ejemplo, 33434+).
  * Recibes ICMP `Time Exceeded` de cada router.
  * Cuando llegas al host, recibes ICMP `Port Unreachable`.
* **ICMP Echo Request** (como `traceroute -I`):

  * Envías paquetes tipo ping.
  * Recibes ICMP Echo Reply al llegar al destino.

Para empezar, te recomiendo **UDP**, porque es lo que se pide en la mayoría de specs de 42.

---

## **3️⃣ Manejo de sockets**

Necesitarás **sockets raw** y/o **sockets UDP** según el enfoque:

* **Raw socket ICMP**:

  * Necesario si quieres recibir ICMP directamente.
  * `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`
  * Requiere **privilegios root**.

* **UDP socket**:

  * Para enviar paquetes UDP hacia puertos altos.
  * `socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)`

* **Time control**:

  * TTL se controla con `setsockopt` (`IPPROTO_IP`, `IP_TTL`).
  * Timeout con `setsockopt` (`SO_RCVTIMEO`).

**Tip:** Envía el paquete desde un socket UDP y recibe ICMP en un socket RAW ICMP, así replicas exactamente cómo funciona `traceroute`.

---

## **4️⃣ Algoritmo principal**

1. **Inicialización**:

   * Parsear argumentos (`host`, `-m max_hops`, `-p port`, etc.).
   * Resolver el hostname a IP (`getaddrinfo`).
2. **Bucle de TTL**:

   ```text
   TTL = 1
   while TTL <= max_hops:
       enviar paquete UDP con TTL
       esperar respuesta ICMP con timeout
       si ICMP recibido:
           imprimir IP y RTT
           si es destino final: break
       si timeout:
           imprimir '*'
       TTL++
   ```
3. **Medición del RTT**:

   * Usar `gettimeofday` antes y después de enviar/recibir.
   * Imprimir en ms con precisión de 1 decimal.
4. **Múltiples intentos por TTL**:

   * Tradicionalmente se envían **3 paquetes por TTL**.
   * Mostrar cada respuesta en una línea con sus RTTs o `*` si timeout.

---

## **5️⃣ Parsing de ICMP**

* Cuando recibes ICMP:

  * **Time Exceeded** → salto intermedio.
  * **Destination Unreachable / Port Unreachable** → destino alcanzado.
* Debes parsear:

  * Cabecera ICMP (`struct icmphdr` en Linux).
  * Cabecera IP original dentro del payload ICMP para saber a qué paquete corresponde.
* Esto asegura que muestras la IP correcta de cada salto.

---

## **6️⃣ Formato de salida**

* Cada línea corresponde a un **TTL**:

  ```
  1  192.168.1.1  1.123 ms  1.234 ms  1.345 ms
  2  10.0.0.1     2.123 ms  2.234 ms  2.345 ms
  3  * * *
  ...
  ```
* Si un paquete no responde → `*`.
* Puedes mostrar el **hostname** además de la IP usando `getnameinfo`, pero esto es opcional según specs de 42.

---

## **7️⃣ Consideraciones importantes**

* **Privilegios root**: Necesitas permisos para raw sockets ICMP.
* **Endianness**: Cuidado al enviar campos de ICMP/UDP en red (usa `htons`, `htonl`).
* **Timeouts**: Usa `select` o `poll` para no bloquear indefinidamente.
* **Señales / Ctrl+C**: Atrapar `SIGINT` para limpiar sockets antes de salir.
* **Errores de red**: Manejar `ECONNREFUSED`, `EHOSTUNREACH`, etc.
* **Compatibilidad IPv4/IPv6**: Al principio, IPv4 es suficiente. IPv6 es opcional.

---

## **8️⃣ Plan de desarrollo paso a paso**

1. Parsear argumentos y resolver host.
2. Crear sockets UDP y RAW ICMP.
3. Implementar envío de paquete con TTL=1.
4. Recibir ICMP y medir RTT.
5. Mostrar primera línea de salida.
6. Iterar TTL hasta destino o max hops.
7. Añadir 3 intentos por TTL.
8. Manejar timeouts y mostrar `*`.
9. Mejorar salida con IP + nombre host opcional.
10. Test exhaustivo con distintos hosts y redes.

---

## **9️⃣ Tests que debes hacer**

* `127.0.0.1` → debería responder en 1 salto.
* Host local en LAN → 2-3 saltos.
* Host público como `google.com` → varios saltos.
* Host inalcanzable → mostrar `*`.
* TTL demasiado bajo → asegurar que no rompe.
* Ctrl+C → limpiar sockets.

---

### **1️⃣ TTL: aclaración precisa**


* **TTL es un contador que se decrementa en cada salto** (cada router que procesa el paquete resta 1).
* **Cuando TTL llega a 0**, el paquete es descartado **por el router actual**, y el router envía un mensaje ICMP "Time Exceeded" al origen.
* Por lo tanto: **TTL no indica el número de routers que atraviesa antes de morir**, sino el **número máximo de routers que puede atravesar**. Si TTL = 3, el paquete puede pasar por hasta 3 routers; si el destino está a 2 routers, llega perfectamente, y el TTL no se “muere” antes. Si está a 4 routers, se descarta en el tercer router.

**Resumen gráfico:**

```
TTL inicial = 3

Origen -> Router 1 (TTL=2)
Router 1 -> Router 2 (TTL=1)
Router 2 -> Router 3 (TTL=0) => paquete descartado, ICMP Time Exceeded enviado al origen
```

* Así funciona `traceroute`: empieza con TTL=1, TTL=2, TTL=3… y cada vez el paquete “muere” en el siguiente router, generando ICMP, que nos permite identificar los saltos.

**TTL limita la cantidad de saltos que puede hacer el paquete, no es un contador exacto de routers recorridos**.

---

### **2️⃣ Diagrama de flujo de ft_traceroute**

```
Inicio
 │
 ├─> Parsear argumentos (host, puerto, max_hops, timeout)
 │
 ├─> Resolver host -> IP
 │
 ├─> Crear socket UDP para enviar paquetes
 │
 ├─> Crear socket RAW ICMP para recibir respuestas
 │
 ├─> TTL = 1
 │
 └─> Mientras TTL <= max_hops
       │
       ├─> Por i=1 a 3 (3 intentos)
       │     ├─> Enviar paquete UDP con TTL actual
       │     └─> Guardar timestamp (t1)
       │
       ├─> Esperar respuesta ICMP con timeout
       │     ├─> Si ICMP recibido
       │     │     ├─> Guardar timestamp (t2)
       │     │     ├─> Calcular RTT = t2 - t1
       │     │     ├─> Analizar tipo ICMP:
       │     │     │     ├─> Time Exceeded → salto intermedio
       │     │     │     └─> Destination Unreachable → destino alcanzado
       │     │     └─> Mostrar IP + RTT
       │     │
       │     └─> Si timeout → mostrar '*'
       │
       ├─> Si destino alcanzado → terminar
       │
       └─> TTL++
 │
 └─> Fin
```

**Notas:**

* Cada “línea” de salida corresponde a un TTL.
* Dentro de cada TTL, puedes tener **3 RTTs o `*` si no hay respuesta**.
* Puedes añadir resolución inversa de IP a hostname opcionalmente.

---¡Vamos a aclararlo con precisión! No te preocupes, ambos conceptos son fundamentales en `ft_traceroute` y otros proyectos de redes.

---

## **1️⃣ RTT (Round-Trip Time)**

* **Definición:**
  RTT es el **tiempo que tarda un paquete en ir desde tu máquina al destino y volver**.

* **Cómo se mide en `traceroute`:**

  1. Guardas el tiempo justo **antes de enviar** el paquete (t1).
  2. Esperas la respuesta ICMP (Time Exceeded o Destination Unreachable).
  3. Guardas el tiempo **al recibir la respuesta** (t2).
  4. Calculas:

[
RTT = t2 - t1
]

* **Formato típico de salida:**

```
192.168.1.1  1.234 ms  1.345 ms  1.456 ms
```

Cada número corresponde a un RTT de un intento de envío.

* **Nota:** Si hay timeout (no llega respuesta) → se muestra `*`.

**Resumen:** RTT mide **latencia total de ida y vuelta**, útil para diagnosticar retrasos en la red.

---

## **2️⃣ Checksum**

* **Definición correcta:**
  El checksum es un **valor de verificación** que asegura que los datos del paquete no se han corrompido durante la transmisión.

* **No es exactamente “codificación”**, sino **una suma de comprobación**:

  * Se calcula sobre **la cabecera del paquete y, en algunos casos, sobre los datos**.
  * Se envía junto con el paquete.
  * El receptor vuelve a calcular el checksum y verifica si coincide.
  * Si coincide → los datos probablemente no se corrompieron.
  * Si no coincide → el paquete se descarta.

* **En ICMP (usado por `traceroute`):**

  * El ICMP Header tiene un campo `checksum`.
  * Se calcula sumando los bytes de la cabecera y del payload (16 bits) y tomando el complemento a uno.
  * Ejemplo de función típica: suma cada par de bytes, agrega overflow y complementa.

* **Resumen sencillo:**

  ```
  Checksum = “firma de integridad” de tu paquete
  Sirve para detectar errores durante el envío
  No altera los datos, solo los verifica
  ```


```
           ┌─────────────────────────────┐
           │        Inicio               │
           └─────────────┬──────────────┘
                         │
             ┌───────────▼───────────┐
             │ Parsear argumentos:   │
             │ host, puerto, max_hops│
             └───────────┬───────────┘
                         │
             ┌───────────▼───────────┐
             │ Resolver host → IP     │
             └───────────┬───────────┘
                         │
             ┌───────────▼───────────┐
             │ Crear socket UDP       │
             │ (para enviar paquetes)│
             └───────────┬───────────┘
                         │
             ┌───────────▼───────────┐
             │ Crear socket RAW ICMP │
             │ (para recibir ICMP)   │
             └───────────┬───────────┘
                         │
             ┌───────────▼───────────┐
             │ TTL = 1               │
             └───────────┬───────────┘
                         │
         ┌───────────────▼────────────────┐
         │ Mientras TTL <= max_hops       │
         └───────────────┬────────────────┘
                         │
        ┌────────────────▼───────────────┐
        │ Por intento = 1 a 3            │
        │  ┌───────────────────────────┐ │
        │  │ Enviar paquete UDP con TTL│ │
        │  └─────────────┬─────────────┘ │
        │                │
        │        ┌───────▼───────────┐
        │        │ Guardar timestamp │
        │        │ (t1)              │
        │        └─────────┬─────────┘
        │                  │
        │        ┌─────────▼─────────┐
        │        │ Esperar ICMP      │
        │        │ con timeout       │
        │        └─────────┬─────────┘
        │                  │
        │        ┌─────────▼──────────┐
        │        │ ICMP recibido?     │
        │        ├───────┬────────────┤
        │        │ Sí    │ No         │
        │          ▼       ▼          │
        │      Guardar t2  Mostrar '*'│
        │      Calcular RTT  (timeout)│
        │        │                    │
        │        ▼                    │
        │ Analizar tipo ICMP          │
        │  ┌────────────────────────┐ │
        │  │ Time Exceeded → salto  │ │
        │  │ Destination Unreachable│ │
        │  │ → destino alcanzado    │ │
        │  └─────────┬──────────────┘ │
        │            │                │
        │        Mostrar IP + RTT     │
        │            │                │
        │     Destino alcanzado? ─────┘
        │            │
        │       Sí ──┴──→ Terminar
        │
        │ TTL++
        │
        └─→ Repetir ciclo hasta max_hops
```

---

### **Puntos clave para implementar según el diagrama**

1. **TTL controlado con `setsockopt(IP_TTL)`**.
2. **Enviar 3 paquetes por TTL** para medición múltiple de RTT.
3. **Usar `gettimeofday` antes y después** de enviar/recibir para calcular RTT.
4. **Parsear ICMP**: Time Exceeded → salto; Destination Unreachable → destino alcanzado.
5. **Timeouts**: manejar con `select` o `poll` para no bloquear.
6. **Mostrar `*` cuando no hay respuesta**.
7. **Iterar hasta llegar al destino o máximo de hops**.

---
