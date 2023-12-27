

<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>
<div class="container mt-5">
    <ul class="nav nav-tabs" id="myTabs">
        <li class="nav-item col-sm-4">
            <a class="nav-link" id="airtel-tab" href='<?php echo site_url("Ecart_Airtel") ?>'>Airtel</a>
        </li>
        <li class="nav-item col-sm-4">
            <a class="nav-link " id="orange-tab" href="<?php echo site_url("Ecart_Orange") ?>">Orange</a>
        </li>
        
        <li class="nav-item col-sm-4">
            <a class="nav-link" id="telma-tab"  href="<?php echo site_url("Ecart_Telma") ?>">Telma</a>
        </li>
    </ul>
</div>
<script>
</script> 