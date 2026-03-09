# Perintah Serial
## Membuat Koneksi WIfi
```
connect,KelilingDunia,12julham12
```
Output
```
[ESP01]: CONNECTED_OK,192.168.0.163
```

## Putus Koneksi
```
disconnect
```
Output:
```
DISCONNECTED
```

## Cek Koneksi
```
check
```
Output
```
[ESP01]: CONNECTED,KelilingDunia,192.168.0.163
```

## Ambil Data
```
get,http://ummuhafidzah.sch.id/iot/get_sensor.php
```

## Kirim Data
```
send,http://ummuhafidzah.sch.id/iot/update_sensor.php?suhu=4&kelembapan=10
```
