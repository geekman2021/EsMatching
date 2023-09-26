<!-- <div class=" container mt-5">

  
    <ul class="nav nav-tabs">
            <li class="nav-item col-sm-4">
                <a class="nav-link" href="#airtel">Airtel</a>
            </li>
            <li class="nav-item col-sm-4">
                <a class="nav-link" href="#orange">Orange</a>
            </li>
            <li class="nav-item col-sm-4">
                <a class="nav-link" href="#telma">Telma</a>
            </li>
    </ul>
</div>

<div id="airtel">
    <form id="addformAirtel" method="post"  action="<?php echo site_url("airtel/importdata"); ?>" enctype="multipart/form-data" >
        <div class="container mt-5">
            <div class="row">
                <div class="form-group col-lg-6">
                    <label for="airtel">Airtel</label>
                    <input type="file" class="form-control" id="input1" name="airtel" onchange="checkFileExtension('input1')" required >
                    <p id="warning1" class="warning-message"></p>
                </div>
                <div class="form-group col-lg-6">
                    <label for="igor">IGOR</label>
                    <input type="file" class="form-control" id="input2" name="igor" onchange="checkFileExtension('input2')" required>
                    <p id="warning2" class="warning-message"></p>
                </div>
            </div>
            <div class="container">
                <div class="text-center">
                    <button type="reset" class="btn btn-warning" id="btn-reinitialiser">Reinitialiser</button>
                    <button type="submit" class="btn btn-success" id="btn-ajouter">Comparer ces deux fichiers</button>
                </div>
            </div>
        </div>
    </form>
</div>

<div id="orange">
    <form id="addformOrange" method="post"  action="<?php echo site_url("orange/importdata"); ?>" enctype="multipart/form-data" >
        <div class="container mt-5">
            <div class="row">
                <div class="form-group col-lg-6">
                    <label for="orange">Orange</label>
                    <input type="file" class="form-control" id="input3" name="orange" onchange="checkFileExtension('input3')" required >
                    <p id="warning1" class="warning-message"></p>
                </div>
                <div class="form-group col-lg-6">
                    <label for="com">Commission</label>
                    <input type="file" class="form-control" id="input4" name="com" onchange="checkFileExtension('input4')" required>
                    <p id="warning2" class="warning-message"></p>
                </div>
                <div class="form-group col-lg-12">
                    <label for="princ">Principale</label>
                    <input type="file" class="form-control" id="input5" name="princ" onchange="checkFileExtension('input5')" required>
                    <p id="warning3" class="warning-message"></p>
                </div>

            </div>
            <div class="container">
                <div class="text-center">
                    <button type="reset" class="btn btn-warning" id="btn-reinitialiser">Reinitialiser</button>
                    <button type="submit" class="btn btn-success" id="btn-ajouter">Comparer ces deux fichiers</button>
                </div>
            </div>
        </div>
    </form>
</div>

<div id="telma">

</div>

<script>

    function checkFileExtension(inputId) {
        var input = document.getElementById(inputId);
        var file = input.files[0];

        var fileName = file.name;
        var fileExtension = fileName.split('.').pop().toLowerCase();
        var warningMessage = document.getElementById('warning' + inputId.charAt(inputId.length - 1)); // Obtenez l'élément de message d'avertissement correspondant

        if (fileExtension === 'xls' || fileExtension === 'csv' || fileExtension=== 'xlsx' ) {
            warningMessage.textContent = '';
            warningMessage.style.display = 'none';
            document.getElementById("btn-ajouter").disabled= false;
        } else {
            warningMessage.textContent = 'Le fichier doit avoir une extension .xls';
            let element= $(".warning-message");

            element.css({
                'color': 'red',
                'font-weight': 'bold',
                'margin-top': '10px',
                'display': 'none'
            });
            warningMessage.style.display = 'block';
            input.value = '';
            document.getElementById("btn-ajouter").disabled= true;
        }



}

</script>



 -->
<div class="container mt-5">
    <ul class="nav nav-tabs" id="myTabs">
        <li class="nav-item col-sm-4">
            <a class="nav-link" id="airtel-tab" href='<?php echo site_url("importer_airtel") ?>'>Airtel</a>
        </li>
        <li class="nav-item col-sm-4">
            <a class="nav-link " id="orange-tab" href="<?php echo site_url("importer_orange") ?>">Orange</a>
        </li>
        
        <li class="nav-item col-sm-4">
            <a class="nav-link" id="telma-tab"  href="<?php echo site_url("importer_telma") ?>">Telma</a>
        </li>
    </ul>
</div>




