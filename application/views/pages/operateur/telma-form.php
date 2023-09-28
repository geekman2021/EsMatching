<form id="addform" method="post"  action="<?php echo site_url("importer_telma/importer"); ?>" enctype="multipart/form-data" >
    <div class="container mt-5">
        <div class="row">
            <div class="form-group col-lg-6">
                <label for="telma">Telma</label>
                <input type="file" class="form-control" id="telma" name="telma">
            </div>
            <div class="form-group col-lg-6">
                <label for="igor">IGOR</label>
                <input type="file" class="form-control" id="igor" name="igor">
            </div>
        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-warning" id="btn-reinitialiser">Reinitialiser</button>
                <button type="submit" class="btn btn-success" id="btn-ajouter">Ajouter</button>
            </div>
        </div>
    </div>
</form>