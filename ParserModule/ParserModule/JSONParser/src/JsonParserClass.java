import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class JsonParserClass {
	
	public static void main(String[] args) {
		//Need the JSON/XML file name and path
		String jsonFilePath = "C:/Users/Monisha/Documents/" + "ddl_json.json";
		//Need the final .c file name and path
		String driverFilePath = "C:/Users/Monisha/Documents/" + "deviceoperations.c";
		//map to keep track of the types of devices in the circuit
		HashMap<String, DeviceStructure> devMap = new HashMap<String, DeviceStructure>();
		HashMap<String, String> inTypeMap = new HashMap<>();
		ArrayList<String> inTypes = new ArrayList<>();
		HashMap<String, String> outTypeMap = new HashMap<>();
		ArrayList<String> outTypes = new ArrayList<>();
		final String MODE_OUT = "out";
		final String MODE_IN = "in";
		final String TYPE_A = "A";
		final String TYPE_D = "D";
		StringBuffer jsonContent = new StringBuffer();
		BufferedReader br = null;

		try {
			String sCurrentLine;

			br = new BufferedReader(new FileReader(jsonFilePath));
			
			while ((sCurrentLine = br.readLine()) != null) {
				jsonContent.append(sCurrentLine);
				System.out.println(sCurrentLine);
			}

		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (br != null)br.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
		
		try {
			System.out.println("Inside");
			JSONObject jObj = new JSONObject(jsonContent.toString());
			JSONObject ddlObj = jObj.getJSONObject("ddl");
			JSONArray devObjs = ddlObj.getJSONArray("device");
			int deviceCount = devObjs.length();
			System.out.println("deviceCount :"+ deviceCount);
			if (deviceCount>0) {
				//Proceed only of the JSON is valid and non-empty
				devMap.clear();
				inTypeMap.clear();
				inTypes.clear();
				outTypeMap.clear();
				outTypes.clear();
				for(int i = 0; i<deviceCount; i++) {
					JSONObject devObj = devObjs.getJSONObject(i);
					//get the pins
					List<PinStructure> pins = new ArrayList<PinStructure>();
					JSONArray pinObjs = devObj.getJSONArray("pin");
					int pinCount = pinObjs.length();
					for (int j = 0; j<pinCount; j++) {
						JSONObject pinObj = pinObjs.getJSONObject(j);
						PinStructure pin = new PinStructure();
						pin.setId(pinObj.getString("id"));
						pin.setIndex(pinObj.getInt("index"));
						pin.setType(pinObj.getString("type"));
						pin.setMode(pinObj.getString("mode"));
						pin.setLevel(pinObj.getString("level"));
						if(MODE_OUT.equals(pin.getMode())) {
							if(TYPE_A.equals(pin.getType())) {
								if(!outTypeMap.containsKey(TYPE_A)) {
									outTypeMap.put(TYPE_A, MODE_OUT);
									outTypes.add(TYPE_A);
									System.out.println("Added A in the outTypes");
								}
							} else if(TYPE_D.equals(pin.getType())) {
								if(!outTypeMap.containsKey(TYPE_D)) {
									outTypeMap.put(TYPE_D, MODE_OUT);
									outTypes.add(TYPE_D);
									System.out.println("Added D in the outTypes");
								}
							}
						} else if(MODE_IN.equals(pin.getMode())) {
							if(TYPE_A.equals(pin.getType())) {
								if(!inTypeMap.containsKey(TYPE_A)) {
									inTypeMap.put(TYPE_A, MODE_OUT);
									inTypes.add(TYPE_A);
									System.out.println("Added A in the inTypes");
								}
							} else if(TYPE_D.equals(pin.getType())) {
								if(!inTypeMap.containsKey(TYPE_D)) {
									inTypeMap.put(TYPE_D, MODE_OUT);
									inTypes.add(TYPE_D);
									System.out.println("Added D in the inTypes");
								}
							}
						}
							
						pins.add(pin);
					} //End of <for> (pin item creation)
					DeviceStructure device = new DeviceStructure();
					device.setDeviceName(devObj.getString("name"));
					device.setPins(pins);
					if(!devMap.containsKey(device.getDeviceName())) { // Assuming there will be only one 
																	  // instance of a device in the 
						                                              // DDL structure in the JSON file
						devMap.put(device.getDeviceName(), device);
					}
				}//End of <for> (Device item creation)
				System.out.println("in count :"+ inTypes.size());
				System.out.println("out count :" + outTypes.size());
			}
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {
			File file = new File(driverFilePath);

			// if file doesn't exists, then create it
			if (!file.exists()) {
				file.createNewFile();
			}

			FileWriter writer = new FileWriter(file.getAbsoluteFile());
			try {
				writer.write("#include <xinu.h>");
				writer.write("\n");
				writer.write("int pread(char dev_type, int dev_id, char *data_buffer){  //Device type ( ADC or GPIO), pin index, pointer to data buffer");
				writer.write("\n");
				writer.write("kprintf(\"In pread. Dev_Type: %c, dev_id : %d, data_buffer : %s \\n\", dev_type, dev_id, data_buffer);");
				writer.write("\n");
				writer.write("	switch (dev_type)  {");
				writer.write("\n");
				//Code to add the types of reads
				int devCount_in = inTypes.size();
				System.out.println("in types:" + devCount_in);
				for(int i = 0; i<devCount_in; i++) {
					switch(inTypes.get(i)) {
						case TYPE_A:
							writer.write("case \'A\' :");
							writer.write("\n");
							writer.write("read(ADC,(char*)data_buffer,4); //ADC type, pointer to data buffer, buffer size");
							writer.write("\n");
							writer.write("break;");
							writer.write("\n");
							break;
						case TYPE_D:
							writer.write("case \'D\' :");
							writer.write("\n");
							writer.write("read_btn(dev_id,(char*)data_buffer, 4);  //Pin index, pointer to data buffer, buffer size");
							writer.write("\n");
							writer.write("break;");
							writer.write("\n");
							break;
					}
				}
				writer.write("default: ");
				writer.write("\n");
				writer.write("kprintf(\"No such type of Device to read\");");
				writer.write("\n");
				writer.write(" }");
				writer.write("\n");
				writer.write("return 0;");
				writer.write("\n");
				writer.write("}");
				writer.write("\n");
				
				writer.write("int pwrite(char dev_type, int dev_id, int data_buffer){  //Device type ( ADC or GPIO), pin index, pointer to data buffer");
				writer.write("\n");
				writer.write("kprintf(\"In pwrite. Dev_Type: %c, dev_id : %d, data_buffer : %d \\n\", dev_type, dev_id, data_buffer);");
				writer.write("\n");
				writer.write("switch (dev_type)  {");
				writer.write("\n");
				//Code to add the types of writes
				int devCount_out = outTypes.size();
				System.out.println("out types:" + devCount_out);
				for(int i = 0; i<devCount_out; i++) {
					switch(outTypes.get(i)) {
						case TYPE_D:
							writer.write("case \'D\' :");
							writer.write("\n");
							writer.write("write_LED(dev_id, data_buffer); //Pin index, pointer to data buffer, buffer size");
							writer.write("\n");
							writer.write("break;");
							writer.write("\n");
							break;
					}
				}
				writer.write("default: ");
				writer.write("\n");
				writer.write("kprintf(\"No such type of Device to write to\");");
				writer.write("\n");
				writer.write("break;");
				writer.write("\n");
				writer.write(" }");
				writer.write("\n");
				writer.write("return 0;");
				writer.write("\n");
				writer.write("}");
				writer.write("\n");
				writer.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}			
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
	
}
