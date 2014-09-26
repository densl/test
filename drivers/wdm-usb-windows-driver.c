
## WDM 功能驱动程序

DriverEntry
	( IN PDRIVER_OBJECT DriverObject,
	  IN PUNICODE_STRING RegistryPath)
{
  DriverObject->DriverExtension->AddDevice = ;
  DriverObject->DriverUnload = ;

  DriverObject->MajorFunction[ IRP_MJ_** ] = ;
  ...etc major function ......

  KeInitializeSpinLock( &BufferLock ); //初始化自旋锁
  
  return ;
}


// 创建设备对象
// 创建符号连接
// 初始化设备扩展， 内存单元复制， 设置读写请求
// 保存设备对象
// 将设备对象连接到设备堆栈
// 初始化内核事件
AddDevice
	( IN PDRIVER_OBJECT DriverObject,
	  IN PDEVICE_OBJECT PhysicalDeviceObject)
{
  RtlInitUnicodeString(
	 &KernelDeviceNameUnicode,
	 KernelDeviceNameBuffer); //符号连接名

  status = IoCreateDevice( DriverObject,  //创建设备对象
		sizeof( DEVICE_EXTENSION ),
		&KernelDeviceNameUnicode,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&fdo); //fdo: PDEVICE_OBJECT


  RtlInitUnicodeString( 
		&UserDeviceLinkUnicode,
		UserDeviceLinkBuffer); //符号连接名

  status = IoCreateSymbolicLink( 
		&UserDeviceLinkUnicode, 
		&KernelDeviceNameUnicode);


  pdx = (PDEVICE_EXTENSION) (  //初始化设备扩展
		fdo->DeviceExtension); //fdo: PDEVICE_OBJECT


  RtlCopyMemory( pdx->DeviceLinkName,  //内存单元复制
		 UserDeviceLinkBuffer, 
		 sizeof(UserDeviceLinkBuffer) );

  //set pdx ...etc .......
  //set fdo flags ...etc ......  //设置读写请求

  pdx->PhysicalDeviceObject = PhysicalDeviceObject; //保存设备对象

  pdx->LowerDeviceObject = //将设备对象连接到设备堆栈
	IoAttachDeviceToDeviceStack( 
		fdo, PhysicalDeviceObject );

  pdx->Usages = 1;

  KeInitializeevent(  //初始化内核事件 
	&pdx->evRemove, NotificationEvent, FALSE );

  return ;
}


//IRP_MJ_PNP
//Get IRP stack location
//Get IRP stack minor function
WDMPnp
	(IN PDEVICE_OBJECT fdo,
	 IN PIRP Irp)
{
	PIO_STACK_LOCATION IrpStack =
		IoGetCurrentIrpStackLocation (Irp);

	switch (IrpStack->MinorFunction)
	//...etc ......
}


//IRP_MJ_CREATE
//递增打开的句柄数  //IRP_MJ_CLOSE 则递减打开的句柄
//设置IRP状态
WDMCreate
	( IN PDEVICE_OBJECT fdo,
	  IN PIRP Irp)
{
  PDEVICE_EXTENSION pdx = ( )fdo->deviceExtension;
 
  pdx->OpenHandles++;  //打开的句柄数递增
  
  Irp->IoStatus.Status = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;  //设置IRP状态

  IoCompleteRequest( Irp, IO_NO_INCREMENT);
}


//
