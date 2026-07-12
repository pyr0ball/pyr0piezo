# 400-Board Production Run Checklist

Complete checklist for flashing 400 Pyr0-Piezo EasyPiezi boards efficiently and reliably.

## Pre-Production Setup (Do This First!)

### Software Setup
- [ ] Install Python dependencies: `pip install pyserial`
- [ ] Verify PlatformIO works: `pio --version`
- [ ] Pre-build firmware: `cd firmware/AVR-Source/Pyr0_Piezo_Sensor_v2.x.x && pio run`
- [ ] Run test script: `cd scripts && python test-flash-setup.py`
- [ ] Test flash one board successfully
- [ ] Backup any existing `flash-log.txt`

### Hardware Preparation
- [ ] Clean pogo pins with isopropyl alcohol
- [ ] Test all pogo pin contacts for continuity
- [ ] Verify ICSP connections (all 6 pins making contact)
- [ ] Verify UART connections (TX, RX, GND making contact)
- [ ] Test USB cable and connection (use USB 3.0 port if possible)
- [ ] Ensure stable power supply

### Spare Parts Ready
- [ ] Spare pogo pin contacts (recommended: 2 complete sets)
- [ ] Spare USB cable
- [ ] Spare programming jig (if available)
- [ ] Cleaning supplies (isopropyl alcohol, lint-free cloth)

### Workspace Setup
- [ ] Comfortable chair (you'll be here 2-4 hours!)
- [ ] Good lighting
- [ ] Organized storage for programmed vs unprogrammed boards
- [ ] Clear labeling system (optional: mark programmed boards)
- [ ] Timer or clock visible

### Optional Improvements
- [ ] 3D print improved clamshell jig (`3D_Models/improved-clamshell-jig.scad`)
- [ ] Set up keyboard macro for auto-ENTER
- [ ] Configure better audio feedback
- [ ] Prepare second workstation for parallel operation

## Production Day Checklist

### Morning Setup (15 minutes)
- [ ] Clean pogo pins
- [ ] Organize 400 boards
- [ ] Set up comfortable work area
- [ ] Start logging: `cd scripts && python batch-flash-enhanced.py --total 400`
- [ ] Flash first 5 boards slowly to verify process
- [ ] Check first 5 boards work correctly

### Every 50 Boards (5 minutes)
- [ ] Clean pogo pins with isopropyl alcohol
- [ ] Check USB connection stability
- [ ] Review flash-log.txt for any failures
- [ ] Take a short break (stretch, hydrate)
- [ ] Verify last few boards work correctly

### Every 100 Boards (10 minutes)
- [ ] Thorough pogo pin inspection
- [ ] Check for pin wear or damage
- [ ] Review failure rate (should be <2%)
- [ ] Consider swapping pogo pins if failures increasing

### Maintenance Alerts
If software shows pogo pin warning (200+ insertions):
- [ ] Stop and clean pins thoroughly
- [ ] Inspect for wear/damage
- [ ] Consider replacing pogo pin set
- [ ] Document in maintenance log

### End of Day
- [ ] Review flash-log.txt
- [ ] Count completed vs failed boards
- [ ] Identify failed boards for re-flash
- [ ] Clean and store jig
- [ ] Backup flash-log.txt: `cp flash-log.txt flash-log-backup-$(date +%Y%m%d).txt`

## Troubleshooting Quick Reference

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Flash fails immediately | ICSP not contacting | Check alignment, clean pins |
| Verification fails | UART not contacting | Check TX/RX pins |
| Intermittent failures | Pogo pin wear | Clean or replace pins |
| USB disconnects | Power/cable issue | Use USB 3.0 port, shorter cable |
| Slow flashing | Baud rate issue | Verify 38400 baud |

## Command Reference

### Standard Run
```bash
cd /Library/Development/devl/pyr0piezo/scripts
python batch-flash-enhanced.py --total 400
```

### Fast Run (auto-start, no ENTER required)
```bash
python batch-flash-enhanced.py --total 400 --auto-start
```

### Resume After Interruption
```bash
# Just run the same command - it auto-resumes from flash-log.txt
python batch-flash-enhanced.py --total 400
```

### Re-flash Failed Boards
```bash
# Extract failed boards from log
grep "FAILED" flash-log.txt

# Flash them individually
python batch-flash-enhanced.py --total [num_failed] --port /dev/ttyUSB0
```

### Manual Single Board Flash
```bash
cd ../firmware/AVR-Source/Pyr0_Piezo_Sensor_v2.x.x
pio run -e ATmega328PB -t upload --upload-port /dev/ttyUSB0
```

## Success Metrics

### Target Performance
- **Throughput**: 60-90 boards/hour (40-60 seconds per board)
- **Success rate**: >98% (less than 8 failures in 400 boards)
- **Jig longevity**: 400+ insertions without pin replacement

### Record Keeping
Track in production log:
- Start time
- End time
- Total completed
- Total failed
- Pogo pin replacements
- Any issues encountered
- Improvement ideas for next run

## After Production

### Quality Check (Sample 10 boards)
- [ ] Power on test
- [ ] Serial communication test (send CONFIG command)
- [ ] Verify firmware version
- [ ] Piezo detection test (if possible)

### Final Steps
- [ ] Archive flash-log.txt with date
- [ ] Document any jig improvements needed
- [ ] Order replacement parts used
- [ ] Note total time vs estimated
- [ ] Calculate actual throughput
- [ ] Update this checklist with lessons learned!

### Re-flash Failures
- [ ] Identify cause of failures from log
- [ ] Re-flash failed boards
- [ ] If still failing, isolate for debugging
- [ ] Document failure modes

## Time Estimates

Based on 40-50 seconds per board average:

| Boards | Minimum Time | Expected Time | With Breaks |
|--------|-------------|---------------|-------------|
| 100    | 67 min      | 83 min        | 100 min     |
| 200    | 133 min     | 167 min       | 200 min     |
| 400    | 267 min     | 333 min       | 400 min     |

**Total for 400 boards: 4-6 hours**

With improvements (auto-start, optimized jig): 3-4 hours

## Resources

- **Main flashing tool**: `scripts/batch-flash-enhanced.py`
- **Test setup**: `scripts/test-flash-setup.py`
- **Jig optimization guide**: `docs/production-jig-optimization.md`
- **Detailed guide**: `scripts/BATCH_FLASHING_GUIDE.md`
- **3D printable jig**: `3D_Models/improved-clamshell-jig.scad`

## Emergency Contacts

- **Discord Support**: https://discord.gg/544587989536473099
- **Documentation**: https://docs.pyroballpcbs.com
- **GitHub Issues**: https://github.com/pyr0ball/pyr0piezo/issues

## Notes / Lessons Learned

(Use this space to document issues and improvements for next time)

```
Date: _______________

Actual time taken: _____________
Boards completed: _____________
Boards failed: _____________
Success rate: _____________%
Pogo pin changes: _____________

Issues encountered:
-
-
-

Improvements for next run:
-
-
-
```

---

**Good luck with your production run! 🚀**
