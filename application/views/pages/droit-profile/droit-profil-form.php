<form id="addForm">
    <div class="container mt-5">
        <div class="row">
            <div class="form-group col-lg-6" style="display:none ;">
                <label for="id">Id</label>
                <input type="text" class="form-control" id="id">
            </div>
            <div class="form-group col-lg-6">
            <label for="">Profile</label>
            <select class="form-control" id="code_profile">
                <?php foreach($profil as $p) { ?>
                <option value="<?php echo $p->code_profile; ?>"><?php echo $p->libelle; ?></option>
                <?php }; ?>
            </select>
            </div>
            <div class="form-group col-lg-6">
                <label for="">droit</label>
                <select class="form-control" id="id_droit">
                    <?php foreach ($droit as $d) { ?>
                        <option value="<?php echo $d->code_droit ;?>"><?php echo $d->permission;  ?></option>
                    <?php }; ?>
                </select>
            </div>
        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-warning"><i class="fa fa-rotate"></i>Reinitialiser</button>
                <button type="submit" class="btn btn-success"><i class="fa fa-plus"></i>Ajouter</button>
            </div>
        </div>
    </div>
</form>

