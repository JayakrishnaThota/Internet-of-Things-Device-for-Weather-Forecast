import java.util.ArrayList;
import java.util.List;

public class DeviceStructure {
	//Name of the Device
	private String deviceName;
	//Pin array
	private List<PinStructure> pins = new ArrayList<PinStructure>();
	
	public String getDeviceName() {
		return deviceName;
	}
	public void setDeviceName(String deviceName) {
		this.deviceName = deviceName;
	}
	public List<PinStructure> getPins() {
		return pins;
	}
	public void setPins(List<PinStructure> pins) {
		this.pins = pins;
	}
}
