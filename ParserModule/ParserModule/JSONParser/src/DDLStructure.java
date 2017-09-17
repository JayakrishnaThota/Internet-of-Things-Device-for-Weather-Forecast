import java.util.ArrayList;
import java.util.List;

//Define the JSON/XML object structure
public class DDLStructure {
	//list of devices
	private List<DeviceStructure> devices = new ArrayList<DeviceStructure>();
	
	public List<DeviceStructure> getDevices() {
		return devices;
	}
	public void setDevices(List<DeviceStructure> devices) {
		this.devices = devices;
	}	
}
