copy "..\app\config\arm5_no_packs\_build\WAP-2-441-Edinburgh.hex" .\
nrfutil settings generate --family NRF52840 --application WAP-2-441-Edinburgh.hex --application-version 3 --bootloader-version 2 --bl-settings-version 1 settings.hex

mergehex.exe -m WAP-2-441-Edinburgh.hex s140_nrf52_7.2.0_softdevice.hex bootloader.hex -o merged_all.hex

REM Production
mergehex.exe -m merged_all.hex settings.hex -o WAP-2-441-Edinburgh-v4.1.0.hex

REM OTA
nrfutil pkg generate --application WAP-2-441-Edinburgh.hex --application-version 0xFF  --hw-version 52 --sd-req 0x0100 --key-file private.pem WAP-2-441-Edinburgh-v4.1.0.zip

rm -rf settings.hex

rm -rf merged_all.hex
